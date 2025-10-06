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
#include "exceptions.hpp"
#include "frame.hpp"
#include "interpreter.hpp"
#include "method.hpp"
#include "system/logger.hpp"
#include "vm.hpp"

using namespace sandvik;

JThread::JThread(Vm& vm_, ClassLoader& classloader_, const std::string& name_)
    : _vm(vm_), _classloader(classloader_), _name(name_), _interpreter(std::make_unique<Interpreter>(*this)) {
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

void JThread::execute() {
	_interpreter->execute();
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