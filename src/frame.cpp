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

#include "frame.hpp"

#include <algorithm>
#include <bit>
#include <stdexcept>

#include "class.hpp"
#include "exceptions.hpp"
#include "method.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

Frame::Frame(Method& method_) : _method(method_) {
	logger.fdebug("new Frame for method = {}.{} registers ={}", method_.getClass().getFullname(), method_.getName(), method_.getNbRegisters());
	_null = Object::makeNull();
	_exception = Object::makeNull();
	_objectReturn = Object::makeNull();
	increaseRegSize(method_.getNbRegisters());
}

void Frame::increaseRegSize(uint32_t size_) {
	if (size_ > _registers.size()) {
		size_t oldSize = _registers.size();
		_registers.resize(size_);
		std::generate(_registers.begin() + oldSize, _registers.end(), []() { return Object::makeNull(); });
	}
}

uint32_t Frame::getDexIdx() const {
	return _method.getClass().getDexIdx();
}

Method& Frame::getMethod() const {
	return _method;
}

uint16_t Frame::pc() const {
	return _pc;
}

uint16_t& Frame::pc() {
	return _pc;
}

void Frame::setPc(uint16_t pc_) {
	_pc = pc_;
}

void Frame::setIntRegister(uint32_t reg, int32_t value) {
	logger.fdebug("setIntRegister: reg={}, value={:x}", reg, value);
	if (reg >= _registers.size()) {
		throw VmException("setIntRegister: reg={} out of bounds", reg);
	}
	_registers[reg] = Object::make(value);
}

int32_t Frame::getIntRegister(uint32_t reg) const {
	if (reg >= _registers.size()) {
		throw VmException("getIntRegister: reg={} out of bounds", reg);
	}
	auto obj = _registers[reg];
	if (obj->isNull()) {
		logger.fdebug("getIntRegister: reg={} -> null", reg);
		return 0;
	}
	if (!obj->isNumberObject()) {
		throw VmException("Register does not contain an NumberObject {}", obj->debug());
	}
	auto number = obj->getValue();
	logger.fdebug("getIntRegister: reg={} -> {}", reg, number);
	return number;
}

void Frame::setLongRegister(uint32_t reg, int64_t value) {
	logger.fdebug("setLongRegister: reg={}, value={:x}, {:x}", reg, static_cast<uint32_t>(value & 0xFFFFFFFF),
	              static_cast<uint32_t>((value >> 32) & 0xFFFFFFFF));
	if (reg + 1 >= _registers.size()) {
		throw VmException("setLongRegister: reg={} out of bounds", reg);
	}
	_registers[reg] = Object::make(static_cast<uint32_t>(value & 0xFFFFFFFF));
	_registers[reg + 1] = Object::make(static_cast<uint32_t>((value >> 32) & 0xFFFFFFFF));
}

int64_t Frame::getLongRegister(uint32_t reg) const {
	logger.fdebug("getLongRegister: reg={}", reg);
	if (reg + 1 >= _registers.size()) {
		throw VmException("getLongRegister: reg={} out of bounds", reg);
	}
	const Object& lsb = *_registers[reg];
	const Object& msb = *_registers[reg + 1];
	if (!lsb.isNumberObject() || !msb.isNumberObject()) {
		throw VmException("Register does not contain valid NumberObjects");
	}
	uint64_t value = static_cast<uint32_t>(msb.getValue());
	value <<= 32;
	value |= static_cast<uint32_t>(lsb.getValue());
	logger.fdebug("getLongRegister: reg={} --> {:x}", reg, value);
	return (int64_t)value;
}

void Frame::setFloatRegister(uint32_t reg, float value) {
	logger.fdebug("setFloatRegister: reg={}, value={}", reg, value);
	if (reg >= _registers.size()) {
		throw VmException("setFloatRegister: reg={} out of bounds", reg);
	}
	uint32_t intValue;
	intValue = std::bit_cast<uint32_t>(value);
	_registers[reg] = Object::make(intValue);
}

float Frame::getFloatRegister(uint32_t reg) const {
	logger.fdebug("getFloatRegister: reg={}", reg);
	if (reg >= _registers.size()) {
		throw VmException("getFloatRegister: reg={} out of bounds", reg);
	}
	const Object& obj = *_registers[reg];
	if (!obj.isNumberObject()) {
		throw VmException("Register does not contain an NumberObject {}", obj.debug());
	}
	auto intValue = static_cast<uint32_t>(obj.getValue());
	return std::bit_cast<float>(intValue);
}

void Frame::setDoubleRegister(uint32_t reg, double value) {
	logger.fdebug("setDoubleRegister: reg={}, value={}", reg, value);
	if (reg + 1 >= _registers.size()) {
		throw VmException("setDoubleRegister: reg={} out of bounds", reg);
	}
	uint64_t temp = std::bit_cast<uint64_t>(value);
	_registers[reg] = Object::make(static_cast<uint32_t>(temp & 0xFFFFFFFF));
	_registers[reg + 1] = Object::make(static_cast<uint32_t>((temp >> 32) & 0xFFFFFFFF));

	logger.fdebug("setDoubleRegister: reg={} value={} {}", reg, _registers[reg]->debug(), _registers[reg + 1]->debug());
}

double Frame::getDoubleRegister(uint32_t reg) const {
	logger.fdebug("getDoubleRegister: reg={}", reg);
	if (reg + 1 >= _registers.size()) {
		throw VmException("getDoubleRegister: reg={} out of bounds", reg);
	}
	const Object& lsb = *_registers[reg];
	const Object& msb = *_registers[reg + 1];
	if (!lsb.isNumberObject() || !msb.isNumberObject()) {
		throw VmException("Register does not contain valid NumberObjects");
	}
	uint64_t value = static_cast<uint32_t>(msb.getValue());
	value <<= 32;
	value |= static_cast<uint32_t>(lsb.getValue());
	double result = std::bit_cast<double>(value);
	return result;
}

void Frame::setObjRegister(uint32_t reg, std::shared_ptr<Object> value) {
	logger.fdebug("setObjRegister: reg={}, obj=<{}>", reg, value->debug());
	if (reg >= _registers.size()) {
		throw VmException("setObjRegister: reg={} out of bounds", reg);
	}
	_registers[reg] = value;
}

std::shared_ptr<Object> Frame::getObjRegister(uint32_t reg) {
	if (reg >= _registers.size()) {
		throw VmException("getObjRegister: reg={} out of bounds", reg);
	}
	logger.fdebug("getObjRegister: reg={} => obj=<{}>", reg, _registers[reg]->debug());
	return _registers[reg];
}

std::shared_ptr<Object> Frame::getException() const {
	return _exception;
}

std::shared_ptr<Object> Frame::getReturnObject() const {
	return _objectReturn;
}

int32_t Frame::getReturnValue() const {
	if (!_objectReturn->isNumberObject()) {
		throw VmException("Return object is not an NumberObject");
	}
	return _objectReturn->getValue();
}

int64_t Frame::getReturnDoubleValue() const {
	if (!_objectReturn->isNumberObject()) {
		throw VmException("Return object is not a isNumberObject");
	}
	return _objectReturn->getLongValue();
}
void Frame::setException(std::shared_ptr<Object> exception_) {
	_exception = exception_;
}
void Frame::setReturnObject(std::shared_ptr<Object> ret_) {
	_objectReturn = ret_;
}
void Frame::setReturnValue(int32_t ret_) {
	_objectReturn = Object::make(ret_);
}
void Frame::setReturnDoubleValue(int64_t ret_) {
	_objectReturn = Object::make(ret_);
	logger.fdebug("setReturnDoubleValue: {:x} -> obj=<{}>", ret_, _objectReturn->debug());
}

void Frame::debug() const {
	logger.fdebug("method={} pc={}", _method.getName(), _pc);
	for (size_t i = 0; i < _registers.size(); ++i) {
		logger.fdebug("register[{}] = {}", i, _registers[i]->debug());
	}
}