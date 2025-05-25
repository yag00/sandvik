#include "object.hpp"

#include <fmt/format.h>

#include <stdexcept>

#include "class.hpp"
#include "system/logger.hpp"

using namespace sandvik;

std::shared_ptr<Object> Object::make(Class& class_) {
	return std::make_shared<ObjectClass>(class_);
}
std::shared_ptr<Object> Object::make(uint64_t number_) {
	return std::make_shared<NumberObject>(number_);
}
std::shared_ptr<Object> Object::make(const std::string& str_) {
	return std::make_shared<StringObject>(str_);
}
std::shared_ptr<Object> Object::make(const std::exception& e_) {
	return std::make_shared<ThrowableObject>(e_);
}
std::shared_ptr<Object> Object::makeNull() {
	return std::make_shared<NullObject>();
}
std::shared_ptr<Object> Object::makeVmObject(const std::string& str_) {
	return std::make_shared<VmObject>(str_);
}

Object::Object(const Object& other) {
	_fields.clear();
	for (const auto& field : other._fields) {
		_fields[field.first] = field.second->clone();
	}
}

bool Object::operator==(const Object& other) const {
	if (this == &other) {
		return true;
	}
	if (typeid(*this) != typeid(other)) {
		return false;
	}
	for (const auto& field : _fields) {
		auto it = other._fields.find(field.first);
		if (it == other._fields.end() || *(field.second) != *(it->second)) {
			return false;
		}
	}
	for (const auto& field : other._fields) {
		if (_fields.find(field.first) == _fields.end()) {
			return false;
		}
	}
	return true;
}
bool Object::operator==(std::nullptr_t) const {
	return false;
}

bool Object::isNumberObject() const {
	return false;
}
bool Object::isArray() const {
	return false;
}
bool Object::isNull() const {
	return false;
}
uint32_t Object::getArrayLength() const {
	return 0;
}

std::string Object::debug() const {
	std::string typeName = typeid(*this).name();
	std::string className = typeid(Class).name();
	return fmt::format("Object: {} Class: {}", typeName, className);
}

bool Object::isInstanceOf(const std::string& instance_) const {
	return false;
}

std::shared_ptr<Object> Object::getField(const std::string& name_) const {
	auto it = _fields.find(name_);
	if (it != _fields.end()) {
		return it->second;
	}
	throw std::out_of_range(fmt::format("Field '{}' does not exist in object {}", name_, this->debug()));
}

void Object::setField(const std::string& name_, std::shared_ptr<Object> value_) {
	_fields[name_] = value_;
}
void Object::setObjectData(uintptr_t* data_) {
	_data.reset(data_);
}

uintptr_t* Object::getObjectData() const {
	return _data.get();
}

///////////////////////////////////////////////////////////////////////////////
NumberObject::NumberObject(uint64_t value_) : _value(value_) {
}
NumberObject::NumberObject(const NumberObject& other) : Object(other), _value(other._value) {
}

bool NumberObject::operator==(const Object& other) const {
	if (this == &other) {
		return true;
	}
	if (!other.isNumberObject()) {
		return false;
	}
	const auto& otherNumber = static_cast<const NumberObject&>(other);
	return _value == otherNumber._value;
}

std::shared_ptr<Object> NumberObject::clone() const {
	return std::make_shared<NumberObject>(*this);
}
int32_t NumberObject::getValue() const {
	return (int32_t)(int64_t)_value;
}
int64_t NumberObject::getLongValue() const {
	return (int64_t)_value;
}
float NumberObject::getFloatValue() const {
	const uint32_t value = (uint32_t)_value;
	return *reinterpret_cast<const float*>(&value);
}
bool NumberObject::isNumberObject() const {
	return true;
}
std::string NumberObject::debug() const {
	return fmt::format("NumberObject: {:#x}", _value);
}
///////////////////////////////////////////////////////////////////////////////
StringObject::StringObject(const std::string& value_) : _value(value_) {
}

StringObject::StringObject(const StringObject& other) : Object(other), _value(other._value) {
}
bool StringObject::operator==(const Object& other) const {
	if (this == &other) {
		return true;
	}
	const auto* otherString = dynamic_cast<const StringObject*>(&other);
	if (otherString == nullptr) {
		return false;
	}
	return _value == otherString->_value;
}

std::shared_ptr<Object> StringObject::clone() const {
	return std::make_shared<StringObject>(*this);
}

std::string StringObject::str() const {
	return _value;
}
std::string StringObject::debug() const {
	return "String=" + _value;
}
///////////////////////////////////////////////////////////////////////////////

ObjectClass::ObjectClass(Class& class_) : _class(class_) {
	for (const auto& fieldname : _class.getFieldList()) {
		auto& field = _class.getField(fieldname);
		if (!field.isStatic()) {
			logger.debug(fmt::format("New instance of {}: Adding field {} type={}", _class.getFullname(), fieldname, field.getType()));
			switch (field.getType()[0]) {
				case 'I':
				case 'Z':
				case 'B':
				case 'S':
				case 'C':
				case 'J':
				case 'F':
				case 'D':
					_fields[fieldname] = std::make_shared<NumberObject>(0);
					break;
				default:
					_fields[fieldname] = std::make_shared<NullObject>();
					break;
			}
		}
	}
	Class* current = &_class;
	while (current->hasSuperClass()) {
		logger.debug(fmt::format("super class {}", current->getSuperClassname()));
		current = &current->getSuperClass();
		for (const auto& fieldname : current->getFieldList()) {
			auto& field = current->getField(fieldname);
			if (!field.isStatic()) {
				logger.debug(fmt::format("New instance of {} inherits from ({}): Adding field {} type={}", _class.getFullname(), current->getFullname(),
				                         fieldname, field.getType()));
				switch (field.getType()[0]) {
					case 'I':
					case 'Z':
					case 'B':
					case 'S':
					case 'C':
					case 'J':
					case 'F':
					case 'D':
						_fields[fieldname] = std::make_shared<NumberObject>(0);
						break;
					default:
						_fields[fieldname] = std::make_shared<NullObject>();
						break;
				}
			}
		}
	}
}

ObjectClass::ObjectClass(const ObjectClass& other) : Object(other), _class(other._class) {
	logger.error("ObjectClass Copy constructor");
}

std::shared_ptr<Object> ObjectClass::clone() const {
	return std::make_shared<ObjectClass>(_class);
}
Class& ObjectClass::getClass() const {
	return _class;
}
std::string ObjectClass::debug() const {
	return "Instance of " + _class.getFullname();
}
bool ObjectClass::isInstanceOf(const std::string& instance_) const {
	return _class.getFullname() == instance_;
}
///////////////////////////////////////////////////////////////////////////////

ThrowableObject::ThrowableObject(const std::exception& e_) : _e(e_) {
}
ThrowableObject::ThrowableObject(const ThrowableObject& other) : Object(other), _e(other._e) {
}

std::shared_ptr<Object> ThrowableObject::clone() const {
	return std::make_shared<ThrowableObject>(_e);
}
void ThrowableObject::throwException() {
	throw _e;
}
std::string ThrowableObject::debug() const {
	return fmt::format("ThrowableObject: {}", _e.what());
}
///////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Object> NullObject::clone() const {
	return std::make_shared<NullObject>();
}
bool NullObject::operator==(std::nullptr_t) const {
	return true;
}
bool NullObject::isNull() const {
	return true;
}
std::string NullObject::debug() const {
	return "Null";
}
///////////////////////////////////////////////////////////////////////////////
VmObject::VmObject(const std::string& instance_) : _instance(instance_) {
}
VmObject::VmObject(const VmObject& other) : Object(other), _instance(other._instance) {
}

std::shared_ptr<Object> VmObject::clone() const {
	return std::make_shared<VmObject>(*this);
}
std::string VmObject::str() const {
	return _instance;
}
std::string VmObject::debug() const {
	return fmt::format("VmObject: {}", _instance);
}
bool VmObject::isInstanceOf(const std::string& instance_) const {
	return _instance == instance_;
}
