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

#include "object.hpp"

#include <fmt/format.h>

#include <stdexcept>

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "field.hpp"
#include "system/logger.hpp"

using namespace sandvik;

ObjectRef Object::make(Class& class_) {
	if (class_.getFullname() == "java.lang.String") {
		return std::make_shared<StringObject>(class_, "");
	}
	return std::make_shared<ObjectClass>(class_);
}
ObjectRef Object::make(uint64_t number_) {
	return std::make_shared<NumberObject>(number_);
}
ObjectRef Object::make(ClassLoader& classloader_, const std::string& str_) {
	auto& clazz = classloader_.getOrLoad("java.lang.String");
	return std::make_shared<StringObject>(clazz, str_);
}
ObjectRef Object::makeNull() {
	return std::make_shared<NullObject>();
}
ObjectRef Object::makeConstClass(ClassLoader& classloader_, Class& classtype_) {
	auto& clazz = classloader_.getOrLoad("java.lang.Class");
	return std::make_shared<ConstClassObject>(clazz, classtype_);
}

ObjectRef Object::makeArray(ClassLoader& classloader_, const Class& classtype_, const std::vector<uint32_t>& dimensions_) {
	return std::make_shared<Array>(classtype_, dimensions_);
}

///////////////////////////////////////////////////////////////////////////////
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

ObjectRef Object::getField(const std::string& name_) const {
	auto it = _fields.find(name_);
	if (it != _fields.end()) {
		return it->second;
	}
	throw std::out_of_range(fmt::format("Field '{}' does not exist in object {}", name_, this->debug()));
}

void Object::setField(const std::string& name_, ObjectRef value_) {
	_fields[name_] = value_;
}

///////////////////////////////////////////////////////////////////////////////
NumberObject::NumberObject(uint64_t value_) : _value(value_) {
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
StringObject::StringObject(Class& class_, const std::string& value_) : ObjectClass(class_), _value(value_) {
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

std::string StringObject::str() const {
	return _value;
}
std::string StringObject::debug() const {
	return "String=" + _value;
}
void StringObject::set(const std::string& str_) {
	_value = str_;
}

///////////////////////////////////////////////////////////////////////////////

ObjectClass::ObjectClass(Class& class_) : _class(class_) {
	for (const auto& fieldname : _class.getFieldList()) {
		auto& field = _class.getField(fieldname);
		if (!field.isStatic()) {
			logger.fdebug("New instance of {}: Adding field {} type={}", _class.getFullname(), fieldname, field.getType());
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
		logger.fdebug("super class {}", current->getSuperClassname());
		current = &current->getSuperClass();
		for (const auto& fieldname : current->getFieldList()) {
			auto& field = current->getField(fieldname);
			if (!field.isStatic()) {
				logger.fdebug("New instance of {} inherits from ({}): Adding field {} type={}", _class.getFullname(), current->getFullname(), fieldname,
				              field.getType());
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
ConstClassObject::ConstClassObject(Class& class_, Class& classtype_) : ObjectClass(class_), _type(classtype_) {
}

bool ConstClassObject::operator==(const Object& other) const {
	if (this == &other) {
		return true;
	}
	const auto* otherClass = dynamic_cast<const ConstClassObject*>(&other);
	if (otherClass == nullptr) {
		return false;
	}
	return _type.getFullname() == otherClass->getClassType().getFullname();
}

Class& ConstClassObject::getClassType() const {
	return _type;
}

std::string ConstClassObject::debug() const {
	return fmt::format("Class<? {}>", _type.getFullname());
}
///////////////////////////////////////////////////////////////////////////////
bool NullObject::operator==(std::nullptr_t) const {
	return true;
}
bool NullObject::isNull() const {
	return true;
}
std::string NullObject::debug() const {
	return "Null";
}
