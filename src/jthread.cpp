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

JThread::JThread(Vm& vm_, ClassLoader& classloader_, const std::string& name_)
    : _vm(vm_), _classloader(classloader_), _name(name_), _interpreter(std::make_unique<Interpreter>(*this)) {
	_thisThread = Object::make(_classloader.getOrLoad("java/lang/Thread"));
	_thisThread->setField("name", Object::make(_classloader, name_));
	_thisThread->setField("priority", Object::make(5));  // normal priority
}

JThread::JThread(Vm& vm_, ClassLoader& classloader_, std::shared_ptr<Object> thread_)
    : _vm(vm_), _classloader(classloader_), _interpreter(std::make_unique<Interpreter>(*this)) {
	_thisThread = thread_;
	_name = _thisThread->getField("name")->str();
	auto target = _thisThread->getField("target");
	if (target == nullptr || target == Object::makeNull()) {
		throw VmException("Thread object has no target Runnable");
	}
	logger.fdebug("Runnable '{}' ", target->debug());
	auto& clazz = target->getClass();
	auto& method = clazz.getMethod("run", "()V");
	Frame& frame = newFrame(method);
	frame.setObjRegister(method.getNbRegisters() - 1, target);
}

JThread::~JThread() {
	if (_thread.joinable()) {
		auto current = std::this_thread::get_id();
		if (_thread.get_id() != current) {
			try {
				_thread.join();
			} catch (const std::exception& ex) {
				logger.ferror("Failed to join thread '{}': {}", _name, ex.what());
			}
		} else {
			logger.fwarning("Destructor invoked from thread '{}' itself; detaching to avoid deadlock", _name);
			_thread.detach();
		}
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

Frame& JThread::newFrame(Method& method_) {
	if (method_.getName() == "<clinit>") {
		auto& clazz = method_.getClass();
		if (!clazz.isStaticInitialized()) {
			// method clinit has been pushed on the stack and will be executed, mark the class as initialized
			clazz.setStaticInitialized();
		} else {
			logger.fwarning("Class {} already initialized", clazz.getFullname());
		}
	}
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

void JThread::run(bool wait_) {
	_thread = std::thread([this]() {
		auto id = std::this_thread::get_id();
		logger.addThread(id, this->_name);
		logger.fdebug("Starting thread '{}'", _name);
		while (!end()) {
			execute();
		}
		logger.fdebug("End of thread '{}'", _name);
		logger.removeThread(id);
	});
	if (wait_) {
		if (_thread.joinable()) {
			_thread.join();
		}
	}
}

void JThread::join() {
	if (_thread.joinable()) {
		_thread.join();
	}
}

void JThread::execute() {
	_interpreter->execute();
}

std::shared_ptr<Object> JThread::getThreadObject() const {
	return _thisThread;
}

std::shared_ptr<Object> JThread::getReturnObject() const {
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

void JThread::setReturnObject(std::shared_ptr<Object> ret_) {
	_objectReturn = ret_;
}

void JThread::setReturnValue(int32_t ret_) {
	_objectReturn = Object::make(ret_);
}

void JThread::setReturnDoubleValue(int64_t ret_) {
	_objectReturn = Object::make(ret_);
}