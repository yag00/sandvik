/*
 * This file is part of Sandvik project.
 * Copyright (C) 2025 Christophe Duvernois
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "jthread.hpp"

#include <fmt/color.h>
#include <fmt/format.h>

#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "frame.hpp"
#include "interpreter.hpp"
#include "method.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "vm.hpp"

using namespace sandvik;

static constexpr int THREAD_STATUS_NEW = 0;
static constexpr int THREAD_STATUS_RUNNABLE = 1;
static constexpr int THREAD_STATUS_TERMINATED = 5;

namespace {
	void notifyMonitorWaiters(ObjectRef obj) {
		if (obj == nullptr || obj == Object::makeNull()) {
			return;
		}

		bool monitorEntered = false;
		try {
			obj->monitorEnter();
			monitorEntered = true;
			obj->notifyAll();
		} catch (...) {
		}
		if (monitorEntered) {
			try {
				obj->monitorExit();
			} catch (...) {
			}
		}
	}
}  // namespace

JThread::JThread(Vm& vm_, ClassLoader& classloader_, const std::string& name_)
    : Thread(name_), _vm(vm_), _classloader(classloader_), _interpreter(std::make_unique<Interpreter>(*this)), _objectReturn(Object::makeNull()) {
	_thisThread = Object::make(_classloader.getOrLoad("java/lang/Thread"));
	_thisThread->setField("name", Object::make(_classloader, name_));
	_thisThread->setField("priority", Object::make(5));  // normal priority
	_thisThread->setField("daemon", Object::make(false));
	_thisThread->setField("threadStatus", Object::make(THREAD_STATUS_NEW));
	_thisThread->setField("eetop", Object::make(0));
	_thisThread->setField("nativePeer", Object::make(0));
	_thisThread->setField("nativeThread", Object::make(0));
	_thisThread->setField("nativeTid", Object::make(0));
}

JThread::JThread(Vm& vm_, ClassLoader& classloader_, ObjectRef thread_)
    : Thread(thread_->getField("name")->str()), _vm(vm_), _classloader(classloader_), _interpreter(std::make_unique<Interpreter>(*this)), _thisThread(thread_) {
	auto target = _thisThread->getField("target");
	if (target == nullptr || target == Object::makeNull()) {
		throw VmException("Thread object has no target Runnable");
	}
	logger.fdebug("Runnable '{}' ", target->toString());
	auto& clazz = target->getClass();
	auto& method = clazz.getMethod("run", "()V");
	Frame& frame = newFrame(method);
	frame.setObjRegister(method.getNbRegisters() - 1, target);
}

JThread::~JThread() {
	// Ensure the worker thread exits while this derived object is still alive.
	// Otherwise Thread's lambda may dispatch virtual calls (done/loop) on a partially
	// destroyed JThread, which triggers "pure virtual method called".
	stop();
	try {
		join();
	} catch (const std::exception& ex) {
		logger.fwarning("Failed to join JThread '{}': {}", getName(), ex.what());
	}
}

Vm& JThread::vm() const {
	return _vm;
}

ClassLoader& JThread::getClassLoader() const {
	return _classloader;
}

bool JThread::end() const {
	return _stack.empty();
}

uint64_t JThread::stackDepth() const {
	return _stack.size();
}

Class& JThread::getStackClass(uint32_t depth_) const {
	if (_childrenThreads.size() > 0) {
		uint32_t depth = depth_;
		for (auto it = _childrenThreads.rbegin(); it != _childrenThreads.rend(); ++it) {
			if (depth >= (*it)->stackDepth()) {
				depth -= (*it)->stackDepth();
			} else {
				return (*it)->getStackClass(depth);
			}
		}
		throw VmException("getStackClass: Stack depth {} out of range", depth_);
	} else {
		if (depth_ >= _stack.size()) {
			throw VmException(fmt::format("Stack depth {} out of range (max {})", depth_, _stack.size() - 1));
		}
		return _stack[_stack.size() - 1 - depth_]->getMethod().getClass();
	}
}

Frame& JThread::newFrame(Method& method_) {
	_stack.push_back(std::make_unique<Frame>(method_));
	return *(_stack.back().get());
}

void JThread::popFrame() {
	_stack.pop_back();
}

Frame& JThread::currentFrame() const {
	if (_stack.empty()) {
		throw VmException("No current frame");
	}
	return *(_stack.back().get());
}

void JThread::loop() {
	try {
		_interpreter->execute();
	} catch (const VmException& e) {
		logger.error(e.what());
		// terminate the whole VM on unhandled exception in thread
		_vm.stop();
		// clear the stack, call to end() will be true
		_stack.clear();
	} catch (const JavaException& e) {
		if (e.getMessage().empty()) {
			logger.ferror("Unhandled Java exception of type {}", e.getExceptionType());
		} else {
			logger.ferror("Unhandled Java exception of type {}: {}", e.getExceptionType(), e.getMessage());
		}
		// terminate the whole VM on unhandled exception in thread
		_vm.stop();
		// clear the stack, call to end() will be true
		_stack.clear();
	}
}

void JThread::onStart() {
	// Keep java.lang.Thread liveness/status coherent for isAlive()/join().
	_thisThread->setField("threadStatus", Object::make((uint64_t)THREAD_STATUS_RUNNABLE));
	_thisThread->setField("eetop", Object::make(1));
	_thisThread->setField("nativePeer", Object::make(1));
	_thisThread->setField("nativeThread", Object::make(1));
	_thisThread->setField("nativeTid", Object::make(1));
	_terminationPublished = false;
}

bool JThread::done() {
	const bool finished = _stack.empty() || !_vm.isRunning();
	if (finished && !_terminationPublished) {
		_thisThread->setField("threadStatus", Object::make((uint64_t)THREAD_STATUS_TERMINATED));
		_thisThread->setField("eetop", Object::make(0));
		_thisThread->setField("nativePeer", Object::make(0));
		_thisThread->setField("nativeThread", Object::make(0));
		_thisThread->setField("nativeTid", Object::make(0));

		// ART-style Thread.join() waits on Thread.lock, so wake that monitor too.
		if (_thisThread->getClass().hasField("lock")) {
			notifyMonitorWaiters(_thisThread->getField("lock"));
		}
		// Also wake waiters using `synchronized(thread)` semantics.
		notifyMonitorWaiters(_thisThread);
		_terminationPublished = true;
	}
	return finished;
}

ObjectRef JThread::getThreadObject() const {
	return _thisThread;
}

ObjectRef JThread::getReturnObject() const {
	return _objectReturn;
}

int32_t JThread::getReturnValue() const {
	if (!_objectReturn->isNumberObject()) {
		throw VmException("Return object is not an NumberObject");
	}
	return _objectReturn->getValue();
}

int64_t JThread::getReturnDoubleValue() const {
	if (!_objectReturn->isNumberObject()) {
		throw VmException("Return object is not a isNumberObject");
	}
	return _objectReturn->getLongValue();
}

void JThread::setReturnObject(ObjectRef ret_) {
	_objectReturn = ret_;
}

void JThread::setReturnValue(int32_t ret_) {
	_objectReturn = Object::make(ret_);
}

void JThread::setReturnDoubleValue(int64_t ret_) {
	_objectReturn = Object::make(ret_);
}

void JThread::visitReferences(const std::function<void(Object*)>& visitor_) const {
	visitor_(_thisThread);
	visitor_(_objectReturn);
	for (const auto& frame : _stack) {
		frame->visitReferences(visitor_);
	}
	for (const auto& child : _childrenThreads) {
		child->visitReferences(visitor_);
	}
}

JThread& JThread::newChild(const std::string& name_) {
	auto child = std::make_unique<JThread>(_vm, _classloader, name_);
	_childrenThreads.push_back(std::move(child));
	return *(_childrenThreads.back());
}

void JThread::popChild() {
	if (_childrenThreads.empty()) {
		throw VmException("No child threads to pop");
	}
	_childrenThreads.pop_back();
}

void JThread::runInCurrentThread() {
	while (!done()) {
		try {
			_interpreter->execute();
		} catch (...) {
			// clear the stack, call to end() will be true
			_stack.clear();
			throw;
		}
	}
}
