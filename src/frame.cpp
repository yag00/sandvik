#include "frame.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <stdexcept>

#include "class.hpp"
#include "method.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

Frame::Frame(Method& method_) : _method(method_), _pc(0) {
	logger.debug(fmt::format("new Frame for method = {}.{} registers ={}", method_.getClass().getFullname(), method_.getName(), method_.getNbRegisters()));
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
	logger.debug(fmt::format("setIntRegister: reg={}, value={:x}", reg, value));
	if (reg >= _registers.size()) {
		throw std::runtime_error(fmt::format("setIntRegister: reg={} out of bounds", reg));
	}
	_registers[reg] = Object::make(value);
}

int32_t Frame::getIntRegister(uint32_t reg) {
	logger.debug(fmt::format("getIntRegister: reg={}", reg));
	if (reg >= _registers.size()) {
		throw std::runtime_error(fmt::format("getIntRegister: reg={} out of bounds", reg));
	}
	auto obj = _registers[reg];
	if (std::dynamic_pointer_cast<NumberObject>(obj)) {
		return std::dynamic_pointer_cast<NumberObject>(obj)->getValue();
	} else {
		logger.debug(fmt::format("getIntRegister: obj ptr=<{:x}>", (uintptr_t)obj.get()));
	}
	if (!obj->isNumberObject()) {
		throw std::runtime_error(fmt::format("Register does not contain an NumberObject {}", obj->debug()));
	}
	return std::dynamic_pointer_cast<NumberObject>(obj)->getValue();
}

void Frame::setLongRegister(uint32_t reg, int64_t value) {
	logger.debug(fmt::format("setLongRegister: reg={}, value={:x}, {:x}", reg, static_cast<uint32_t>(value & 0xFFFFFFFF),
	                         static_cast<uint32_t>((value >> 32) & 0xFFFFFFFF)));
	if (reg + 1 >= _registers.size()) {
		throw std::runtime_error(fmt::format("setLongRegister: reg={} out of bounds", reg));
	}
	_registers[reg] = Object::make(static_cast<uint32_t>(value & 0xFFFFFFFF));
	_registers[reg + 1] = Object::make(static_cast<uint32_t>((value >> 32) & 0xFFFFFFFF));
}

int64_t Frame::getLongRegister(uint32_t reg) {
	logger.debug(fmt::format("getLongRegister: reg={}", reg));
	if (reg + 1 >= _registers.size()) {
		throw std::runtime_error(fmt::format("setLongRegister: reg={} out of bounds", reg));
	}
	Object& lsb = *_registers[reg];
	Object& msb = *_registers[reg + 1];
	if (!lsb.isNumberObject() || !msb.isNumberObject()) {
		throw std::runtime_error("Register does not contain valid NumberObjects");
	}
	uint64_t value = static_cast<uint32_t>(static_cast<NumberObject&>(msb).getValue());
	value <<= 32;
	value |= static_cast<uint32_t>(static_cast<NumberObject&>(lsb).getValue());
	logger.debug(fmt::format("getLongRegister: reg={} --> {:x}", reg, value));
	return (int64_t)value;
}

void Frame::setFloatRegister(uint32_t reg, float value) {
	logger.debug(fmt::format("setFloatRegister: reg={}, value={}", reg, value));
	if (reg >= _registers.size()) {
		throw std::runtime_error(fmt::format("setFloatRegister: reg={} out of bounds", reg));
	}
	_registers[reg] = Object::make(*reinterpret_cast<uint32_t*>(&value));
}

float Frame::getFloatRegister(uint32_t reg) {
	logger.debug(fmt::format("getFloatRegister: reg={}", reg));
	if (reg >= _registers.size()) {
		throw std::runtime_error(fmt::format("getFloatRegister: reg={} out of bounds", reg));
	}
	Object& obj = *_registers[reg];
	if (!obj.isNumberObject()) {
		throw std::runtime_error(fmt::format("Register does not contain an NumberObject {}", obj.debug()));
	}
	uint32_t intValue = static_cast<uint32_t>(static_cast<NumberObject&>(obj).getValue());
	return *reinterpret_cast<float*>(&intValue);
}

void Frame::setDoubleRegister(uint32_t reg, double value) {
	logger.debug(fmt::format("setDoubleRegister: reg={}, value={}", reg, value));
	if (reg + 1 >= _registers.size()) {
		throw std::runtime_error(fmt::format("setDoubleRegister: reg={} out of bounds", reg));
	}
	uint64_t temp = *reinterpret_cast<uint64_t*>(&value);
	_registers[reg] = Object::make(static_cast<uint32_t>(temp & 0xFFFFFFFF));
	_registers[reg + 1] = Object::make(static_cast<uint32_t>((temp >> 32) & 0xFFFFFFFF));

	logger.debug(fmt::format("setDoubleRegister: reg={} value={} {}", reg, _registers[reg]->debug(), _registers[reg + 1]->debug()));
}

double Frame::getDoubleRegister(uint32_t reg) {
	logger.debug(fmt::format("getDoubleRegister: reg={}", reg));
	if (reg + 1 >= _registers.size()) {
		throw std::runtime_error(fmt::format("getDoubleRegister: reg={} out of bounds", reg));
	}
	Object& lsb = *_registers[reg];
	Object& msb = *_registers[reg + 1];
	if (!lsb.isNumberObject() || !msb.isNumberObject()) {
		throw std::runtime_error("Register does not contain valid NumberObjects");
	}
	uint64_t value = static_cast<uint32_t>(static_cast<NumberObject&>(msb).getValue());
	value <<= 32;
	value |= static_cast<uint32_t>(static_cast<NumberObject&>(lsb).getValue());
	return *reinterpret_cast<double*>(&value);
}

void Frame::setObjRegister(uint32_t reg, std::shared_ptr<Object> value) {
	logger.debug(fmt::format("setObjRegister: reg={}, obj=<{}>", reg, value->debug()));
	if (reg >= _registers.size()) {
		throw std::runtime_error(fmt::format("setObjRegister: reg={} out of bounds", reg));
	}
	_registers[reg] = value;
}
/*
void Frame::setObjRegister(uint32_t reg, std::shared_ptr<Object>&& value) {
    logger.debug(fmt::format("setObjRegister: reg={}, obj=<{}>", reg, value->debug()));
    if (reg >= _registers.size()) {
        increaseRegSize(reg + 1);
    }
    _registers[reg] = std::move(value);
}
*/

std::shared_ptr<Object> Frame::getObjRegister(uint32_t reg) {
	if (reg >= _registers.size()) {
		throw std::runtime_error(fmt::format("getObjRegister: reg={} out of bounds", reg));
	}
	logger.debug(fmt::format("getObjRegister: reg={} => obj=<{}>", reg, _registers[reg]->debug()));
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
		throw std::runtime_error("Return object is not an NumberObject");
	}
	return static_cast<NumberObject&>(*_objectReturn).getValue();
}

int64_t Frame::getReturnDoubleValue() const {
	if (!_objectReturn->isNumberObject()) {
		throw std::runtime_error("Return object is not a isNumberObject");
	}
	return static_cast<NumberObject&>(*_objectReturn).getLongValue();
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
	logger.debug(fmt::format("setReturnDoubleValue: {:x} -> obj=<{}>", ret_, _objectReturn->debug()));
}

void Frame::debug() const {
	logger.debug(fmt::format("method={} pc={}", _method.getName(), _pc));
	for (size_t i = 0; i < _registers.size(); ++i) {
		logger.debug(fmt::format("register[{}] = {}", i, _registers[i]->debug()));
	}
}