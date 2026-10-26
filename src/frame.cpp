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
	logger.ftrace("new Frame for method={}.{} registers={}", method_.getClass().getFullname(), method_.getName(), method_.getNbRegisters());
	if (!method_.isStatic() && method_.getNbRegisters() == 0) {
		throw VmException("Method {}.{} has zero registers", method_.getClass().getFullname(), method_.getName());
	}
	_exception = Object::makeNull();
	increaseRegSize(method_.getNbRegisters());
}

void Frame::increaseRegSize(uint32_t size_) {
	if (size_ > _registers.size()) {
		// New slots default-construct as RegisterSlot::Kind::Null - no allocation involved.
		_registers.resize(size_);
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

bool Frame::slotAsRaw32(const RegisterSlot& slot_, uint32_t& out_) {
	if (slot_.kind == RegisterSlot::Kind::Raw) {
		out_ = static_cast<uint32_t>(slot_.raw);
		return true;
	}
	if (slot_.kind == RegisterSlot::Kind::Object && slot_.obj->isNumberObject()) {
		out_ = static_cast<uint32_t>(slot_.obj->getValue());
		return true;
	}
	return false;
}

uint64_t Frame::getRawLongRegister(uint32_t reg) const {
	if (reg + 1 >= _registers.size()) {
		throw VmException("getRawLongRegister: reg={} out of bounds", reg);
	}
	uint32_t lo = 0;
	uint32_t hi = 0;
	if (!slotAsRaw32(_registers[reg], lo) || !slotAsRaw32(_registers[reg + 1], hi)) {
		throw VmException("Register does not contain valid NumberObjects");
	}
	uint64_t value = hi;
	value <<= 32;
	value |= lo;
	return value;
}

void Frame::setRawLongRegister(uint32_t reg, uint64_t value) {
	logger.fverbose("v{}:v{} <- {:#x}", reg, reg + 1, value);
	storeWideRegister(reg, value);
}

void Frame::storeWideRegister(uint32_t reg, uint64_t value) {
	if (reg + 1 >= _registers.size()) {
		throw VmException("setRawLongRegister: reg={} out of bounds", reg);
	}
	_registers[reg] = RegisterSlot::makeRaw(value & 0xFFFFFFFFu);
	_registers[reg + 1] = RegisterSlot::makeRaw((value >> 32) & 0xFFFFFFFFu);
}

void Frame::setIntRegister(uint32_t reg, int32_t value) {
	logger.fverbose("v{} <- {} ({:#x})", reg, value, static_cast<uint32_t>(value));
	if (reg >= _registers.size()) {
		throw VmException("setIntRegister: reg={} out of bounds", reg);
	}
	_registers[reg] = RegisterSlot::makeRaw(static_cast<uint32_t>(value));
}

int32_t Frame::getIntRegister(uint32_t reg) const {
	if (reg >= _registers.size()) {
		throw VmException("getIntRegister: reg={} out of bounds", reg);
	}
	const auto& slot = _registers[reg];
	uint32_t raw = 0;
	if (slotAsRaw32(slot, raw)) {
		auto number = static_cast<int32_t>(raw);
		logger.fverbose("v{} -> {} ({:#x})", reg, number, static_cast<uint32_t>(number));
		return number;
	}
	// A well-formed dex file never reads a register before writing it on every possible path
	// (the Dalvik verifier enforces definite assignment); sandvik has no verifier of its own, so
	// reaching this means either malformed/adversarial bytecode or a bug in the interpreter. Fail
	// loudly rather than silently synthesizing a value - consistent with
	// getFloatRegister/getRawLongRegister/getReturnValue/getReturnDoubleValue, which already throw
	// in the equivalent case.
	if (slot.kind == RegisterSlot::Kind::Null) {
		throw VmException("getIntRegister: reg={} was never written", reg);
	}
	throw VmException("Register does not contain an NumberObject {}", slot.obj->toString());
}

void Frame::setLongRegister(uint32_t reg, int64_t value) {
	logger.fverbose("v{}:v{} <- {} ({:#x})", reg, reg + 1, value, static_cast<uint64_t>(value));
	storeWideRegister(reg, (uint64_t)value);
}

int64_t Frame::getLongRegister(uint32_t reg) const {
	uint64_t value = getRawLongRegister(reg);
	logger.fverbose("v{}:v{} -> {} ({:#x})", reg, reg + 1, static_cast<int64_t>(value), value);
	return (int64_t)value;
}

void Frame::setFloatRegister(uint32_t reg, float value) {
	logger.fverbose("v{} <- {} (float)", reg, value);
	if (reg >= _registers.size()) {
		throw VmException("setFloatRegister: reg={} out of bounds", reg);
	}
	uint32_t intValue = std::bit_cast<uint32_t>(value);
	_registers[reg] = RegisterSlot::makeRaw(intValue);
}

float Frame::getFloatRegister(uint32_t reg) const {
	if (reg >= _registers.size()) {
		throw VmException("getFloatRegister: reg={} out of bounds", reg);
	}
	const auto& slot = _registers[reg];
	uint32_t intValue = 0;
	if (!slotAsRaw32(slot, intValue)) {
		throw VmException("Register does not contain an NumberObject");
	}
	logger.fverbose("v{} -> {} (float)", reg, std::bit_cast<float>(intValue));
	return std::bit_cast<float>(intValue);
}

void Frame::setDoubleRegister(uint32_t reg, double value) {
	logger.fverbose("v{}:v{} <- {} (double)", reg, reg + 1, value);
	storeWideRegister(reg, std::bit_cast<uint64_t>(value));
}

double Frame::getDoubleRegister(uint32_t reg) const {
	uint64_t value = getRawLongRegister(reg);
	logger.fverbose("v{}:v{} -> {} (double)", reg, reg + 1, std::bit_cast<double>(value));
	return std::bit_cast<double>(value);
}

void Frame::setObjRegister(uint32_t reg, ObjectRef value) {
	logger.fverbose("v{} <- {}", reg, describe(value));
	if (reg >= _registers.size()) {
		throw VmException("setObjRegister: reg={} out of bounds", reg);
	}
	_registers[reg] = RegisterSlot::makeObject(value);
}

ObjectRef Frame::getObjRegister(uint32_t reg) {
	if (reg >= _registers.size()) {
		throw VmException("getObjRegister: reg={} out of bounds", reg);
	}
	const auto& slot = _registers[reg];
	if (slot.kind == RegisterSlot::Kind::Null) {
		return Object::makeNull();
	}
	if (slot.kind == RegisterSlot::Kind::Raw) {
		// Defensive fallback for a type-mismatched read (should not happen with well-formed
		// bytecode): box the raw value on the fly, mirroring the old behavior where every
		// register held a boxed Object regardless of the accessor used to read it.
		return Object::make(slot.raw);
	}
	logger.fverbose("v{} -> {}", reg, describe(slot.obj));
	return slot.obj;
}

ObjectRef Frame::getException() const {
	return _exception;
}

ObjectRef Frame::getReturnObject() const {
	if (_returnSlot.kind == RegisterSlot::Kind::Null) {
		return Object::makeNull();
	}
	if (_returnSlot.kind == RegisterSlot::Kind::Raw) {
		return Object::make(_returnSlot.raw);
	}
	return _returnSlot.obj;
}

int32_t Frame::getReturnValue() const {
	if (_returnSlot.kind == RegisterSlot::Kind::Raw) {
		return static_cast<int32_t>(_returnSlot.raw);
	}
	if (_returnSlot.kind == RegisterSlot::Kind::Object && _returnSlot.obj->isNumberObject()) {
		return _returnSlot.obj->getValue();
	}
	throw VmException("Return object is not an NumberObject");
}

int64_t Frame::getReturnDoubleValue() const {
	if (_returnSlot.kind == RegisterSlot::Kind::Raw) {
		return static_cast<int64_t>(_returnSlot.raw);
	}
	if (_returnSlot.kind == RegisterSlot::Kind::Object && _returnSlot.obj->isNumberObject()) {
		return _returnSlot.obj->getLongValue();
	}
	throw VmException("Return object is not a isNumberObject");
}
void Frame::setException(ObjectRef exception_) {
	_exception = exception_;
}
void Frame::setReturnObject(ObjectRef ret_) {
	_returnSlot = RegisterSlot::makeObject(ret_);
}
void Frame::setReturnValue(int32_t ret_) {
	_returnSlot = RegisterSlot::makeRaw(static_cast<uint32_t>(ret_));
}
void Frame::setReturnDoubleValue(int64_t ret_) {
	_returnSlot = RegisterSlot::makeRaw(static_cast<uint64_t>(ret_));
	logger.fverbose("return <- {} ({:#x}) (wide)", ret_, static_cast<uint64_t>(ret_));
}

void Frame::debug() const {
	logger.fdebug("method={} pc={}", _method.getName(), _pc);
	for (size_t i = 0; i < _registers.size(); ++i) {
		const auto& slot = _registers[i];
		switch (slot.kind) {
			case RegisterSlot::Kind::Null:
				logger.fdebug("register[{}] = null", i);
				break;
			case RegisterSlot::Kind::Raw:
				logger.fdebug("register[{}] = raw:{:x}", i, slot.raw);
				break;
			case RegisterSlot::Kind::Object:
				logger.fdebug("register[{}] = {}", i, slot.obj->toString());
				break;
		}
	}
}

void Frame::visitReferences(const std::function<void(Object*)>& visitor_) const {
	if (_returnSlot.kind == RegisterSlot::Kind::Object && _returnSlot.obj != nullptr) {
		visitor_(_returnSlot.obj);
	}
	if (_exception != nullptr) {
		visitor_(_exception);
	}
	for (const auto& reg : _registers) {
		if (reg.kind == RegisterSlot::Kind::Object && reg.obj != nullptr) {
			visitor_(reg.obj);
		}
	}
}
bool Frame::isHandlingException(ObjectRef exception_) const {
	return _exception == exception_;
}
