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

namespace sandvik {
	class NumberObject : public Object {
		public:
			NumberObject(uint64_t value_);
			~NumberObject() override = default;

			bool isNumberObject() const override;
			int32_t getValue() const override;
			int64_t getLongValue() const override;
			std::string debug() const override;

			bool operator==(const Object& other) const override;

		private:
			uint64_t _value;
	};
	class ObjectClass : public Object {
		public:
			ObjectClass(Class& class_);
			~ObjectClass() override = default;

			bool isInstanceOf(const std::string& instance_) const override;

			bool isClass() const override;
			Class& getClass() const override;
			std::string debug() const override;

		private:
			Class& _class;
	};
	class ConstClassObject : public ObjectClass {
		public:
			ConstClassObject(Class& class_, Class& type_);
			~ConstClassObject() override = default;

			const Class& getClassType() const override;
			std::string debug() const override;

			bool operator==(const Object& other) const override;

		private:
			Class& _type;
	};
	class StringObject : public ObjectClass {
		public:
			StringObject(Class& class_, const std::string& value_);
			~StringObject() override = default;

			bool isString() const override;
			std::string str() const override;
			void setString(const std::string& str_) override;
			std::string debug() const override;

			bool operator==(const Object& other) const override;

		private:
			std::string _value;
	};
	class NullObject : public Object {
		public:
			NullObject() = default;
			~NullObject() override = default;

			bool operator==(std::nullptr_t) const override;
			bool isNull() const override;
			std::string debug() const override;
	};
}  // namespace sandvik

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

int32_t Object::getValue() const {
	throw std::bad_cast();
}

int64_t Object::getLongValue() const {
	throw std::bad_cast();
}

bool Object::isArray() const {
	return false;
}
uint32_t Object::getArrayLength() const {
	throw std::bad_cast();
}

bool Object::isNull() const {
	return false;
}

bool Object::isClass() const {
	return false;
}
Class& Object::getClass() const {
	throw std::bad_cast();
}

bool Object::isString() const {
	return false;
}

std::string Object::str() const {
	throw std::bad_cast();
}

void Object::setString(const std::string& str_) {
	throw std::bad_cast();
}

const Class& Object::getClassType() const {
	throw std::bad_cast();
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

bool StringObject::isString() const {
	return true;
}
std::string StringObject::str() const {
	return _value;
}
std::string StringObject::debug() const {
	return "String=" + _value;
}
void StringObject::setString(const std::string& str_) {
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
bool ObjectClass::isClass() const {
	return true;
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

const Class& ConstClassObject::getClassType() const {
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
