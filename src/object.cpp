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

#include <xxhash.h>

#include <fmt/format.h>

#include <atomic>
#include <stdexcept>

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "field.hpp"
#include "monitor.hpp"
#include "system/logger.hpp"

namespace sandvik {
	/** @brief Number object representing integer values. */
	class NumberObject : public Object {
		public:
			/** Constructor for NumberObject.
			 * @param value_ The integer value to be stored in the NumberObject.
			 */
			explicit NumberObject(uint64_t value_);
			~NumberObject() override = default;

			/**
			 * @brief Checks if the object represents a number.
			 * @return True if the object is a number, false otherwise.
			 */
			bool isNumberObject() const override;

			/**
			 * @brief Gets the integer value of the object.
			 * @return Integer value.
			 * @throw std::bad_cast if the object is not a number.
			 */
			int32_t getValue() const override;
			/**
			 * @brief Gets the long integer value of the object.
			 * @return Long integer value.
			 * @throw std::bad_cast if the object is not a number.
			 */
			int64_t getLongValue() const override;

			/**
			 * @brief Returns a debug string representation of the object.
			 * @return Debug string.
			 */
			std::string debug() const override;

			/**
			 * @brief Atomically sets to the given value and returns the old value.
			 *
			 * @param newValue the new value
			 * @return the previous value
			 */
			int32_t getAndSet(int32_t newValue) override;
			/**
			 * @brief Atomically sets to the given value and returns the old value.
			 *
			 * @param newValue the new value
			 * @return the previous value
			 */
			int64_t getAndSet(int64_t newValue) override;
			/**
			 * @brief Atomically adds the given value to the current value.
			 *
			 * @param delta the value to add
			 * @return the previous value
			 */
			int32_t getAndAdd(int32_t delta) override;
			/**
			 * @brief Atomically adds the given value to the current value.
			 *
			 * @param delta the value to add
			 * @return the previous value
			 */
			int64_t getAndAdd(int64_t delta) override;
			/**
			 * @brief Atomically adds the given value to the current value.
			 *
			 * @param delta the value to add
			 * @return the updated value
			 */
			int32_t addAndGet(int32_t delta) override;
			/**
			 * @brief Atomically adds the given value to the current value.
			 *
			 * @param delta the value to add
			 * @return the updated value
			 */
			int64_t addAndGet(int64_t delta) override;
			/**
			 * @brief Atomically sets the value to the given updated value
			 * if the current value == the expected value.
			 *
			 * @param expect the expected value
			 * @param update the new value
			 * @return true if successful. False return indicates that
			 * the actual value was not equal to the expected value.
			 */
			bool compareAndSet(int32_t expect, int32_t update) override;
			/**
			 * @brief Atomically sets the value to the given updated value
			 * if the current value == the expected value.
			 *
			 * @param expect the expected value
			 * @param update the new value
			 * @return true if successful. False return indicates that
			 * the actual value was not equal to the expected value.
			 */
			bool compareAndSet(int64_t expect, int64_t update) override;
			/**
			 * @brief Atomically sets the value to the given updated value
			 * if the current value == the expected value.
			 *
			 * May fail spuriously and does not provide ordering guarantees, so is
			 * only rarely an appropriate alternative to compareAndSet.
			 *
			 * @param expect the expected value
			 * @param update the new value
			 * @return true if successful
			 */
			bool weakCompareAndSet(int32_t expect, int32_t update) override;
			/**
			 * @brief Atomically sets the value to the given updated value
			 * if the current value == the expected value.
			 *
			 * May fail spuriously and does not provide ordering guarantees, so is
			 * only rarely an appropriate alternative to compareAndSet.
			 *
			 * @param expect the expected value
			 * @param update the new value
			 * @return true if successful
			 */
			bool weakCompareAndSet(int64_t expect, int64_t update) override;

			/**
			 * @brief Equality operator for comparing with another Object.
			 * @param other Reference to the other Object.
			 * @return True if objects are equal, false otherwise.
			 */
			bool operator==(const Object& other) const override;

		private:
			std::atomic<uint64_t> _value;
	};
	/** @brief Object class representing Java Class objects. */
	class ObjectClass : public Object {
		public:
			/** Constructor for ObjectClass.
			 * @param class_ Reference to the Class object.
			 */
			explicit ObjectClass(Class& class_);
			~ObjectClass() override = default;

			/**
			 * @brief Checks if the object is an instance of the specified type.
			 * @param instance_ Name of the type to check.
			 * @return True if the object is an instance of the specified type, false otherwise.
			 */
			bool isInstanceOf(const std::string& instance_) const override;
			/**
			 * @brief Checks if the object is a class object (instance of java.lang.Class).
			 * @return True if the object is a class object, false otherwise.
			 * @throw std::bad_cast if object is not a class.
			 */
			bool isClass() const override;
			/**
			 * @brief Gets the Class of the object.
			 * @return Reference to the Class object.
			 * @throw std::bad_cast if the object does not have a class.
			 */
			Class& getClass() const override;
			/**
			 * @brief Returns a debug string representation of the object.
			 * @return Debug string.
			 */
			std::string debug() const override;

		private:
			Class& _class;
	};
	/** @brief Constant class object representing Java Class<?> objects. */
	class ConstClassObject : public ObjectClass {
		public:
			/** Constructor for ConstClassObject.
			 * @param class_ Reference to the Class object.
			 * @param classtype_ Reference to the Class type.
			 */
			ConstClassObject(Class& class_, Class& classtype_);
			~ConstClassObject() override = default;

			const Class& getClassType() const override;
			/**
			 * @brief Returns a debug string representation of the object.
			 * @return Debug string.
			 */
			std::string debug() const override;
			/**
			 * @brief Equality operator for comparing with another Object.
			 * @param other Reference to the other Object.
			 * @return True if objects are equal, false otherwise.
			 */
			bool operator==(const Object& other) const override;

			/** @brief Enter monitor
			 *
			 * need to override monitor methods to lock the class object for static fields access
			 */
			void monitorEnter() override;
			/** @brief Exit monitor
			 *
			 * need to override monitor methods to lock the class object for static fields access
			 */
			void monitorExit() override;

		private:
			Class& _type;
	};
	/** @brief String object representing Java String objects. */
	class StringObject : public ObjectClass {
		public:
			/** Constructor for StringObject.
			 * @param class_ Reference to the Class object.
			 * @param value_ String value to initialize the object.
			 */
			StringObject(Class& class_, const std::string& value_);
			~StringObject() override = default;

			/**
			 * @brief  Checks if the object is a string.
			 * @return True if the object is a string, false otherwise.
			 */
			bool isString() const override;
			/**
			 * @brief Gets the string value of the object.
			 * @return String value.
			 * @throw std::bad_cast if the object is not a string.
			 */
			std::string str() const override;
			/**
			 * @brief Set the string value of the object.
			 * @param str_ String value.
			 * @throw std::bad_cast if the object is not a string.
			 */
			void setString(const std::string& str_) override;

			/**
			 * @brief Returns a debug string representation of the object.
			 * @return Debug string.
			 */
			std::string debug() const override;
			/**
			 * @brief Equality operator for comparing with another Object.
			 * @param other Reference to the other Object.
			 * @return True if objects are equal, false otherwise.
			 */
			bool operator==(const Object& other) const override;

		private:
			std::string _value;
	};
	/** @brief Null object representing Java null references. */
	class NullObject : public Object {
		public:
			NullObject() = default;
			~NullObject() override = default;
			/**
			 * @brief Equality operator for comparing with nullptr.
			 * @return True if the object is null, false otherwise.
			 */
			bool operator==(std::nullptr_t) const override;
			/**
			 * @brief Checks if the object is null.
			 * @return True if the object is null, false otherwise.
			 */
			bool isNull() const override;

			/**
			 * @brief Returns a debug string representation of the object.
			 * @return Debug string.
			 */
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
Object::Object() : _monitor(std::make_unique<Monitor>()) {
}

bool Object::operator==(const Object& other) const {
	// Same pointer -> equal
	if (this == &other) return true;
	// Both are numbers -> compare numeric values
	if (this->isNumberObject() && other.isNumberObject()) {
		return this->getValue() == other.getValue();
	}
	// Different types -> not equal
	return false;
}

bool Object::operator==(std::nullptr_t) const {
	return false;
}

void Object::monitorEnter() {
	_monitor->enter();
}

void Object::monitorExit() {
	_monitor->exit();
}

void Object::monitorCheck() const {
	_monitor->check();
}

void Object::wait(uint64_t timeout_) {
	_monitor->wait(timeout_);
}

void Object::notify() {
	_monitor->notify();
}

void Object::notifyAll() {
	_monitor->notifyAll();
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

int32_t Object::getAndSet(int32_t newValue) {
	throw std::bad_cast();
}
int64_t Object::getAndSet(int64_t newValue) {
	throw std::bad_cast();
}
int32_t Object::getAndAdd(int32_t delta) {
	throw std::bad_cast();
}
int64_t Object::getAndAdd(int64_t delta) {
	throw std::bad_cast();
}
int32_t Object::addAndGet(int32_t delta) {
	throw std::bad_cast();
}
int64_t Object::addAndGet(int64_t delta) {
	throw std::bad_cast();
}
bool Object::compareAndSet(int32_t expect, int32_t update) {
	throw std::bad_cast();
}
bool Object::compareAndSet(int64_t expect, int64_t update) {
	throw std::bad_cast();
}
bool Object::weakCompareAndSet(int32_t expect, int32_t update) {
	throw std::bad_cast();
}
bool Object::weakCompareAndSet(int64_t expect, int64_t update) {
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

int32_t Object::identityHashCode() const {
	uint64_t ptr = reinterpret_cast<uint64_t>(this);
	return static_cast<int32_t>(XXH32(&ptr, sizeof(ptr), 0));
}

bool Object::isInstanceOf(const std::string& instance_) const {
	return false;
}

ObjectRef Object::getField(const std::string& name_) const {
	monitorCheck();  // Ensure the current thread owns the monitor (if locked)
	auto it = _fields.find(name_);
	if (it != _fields.end()) {
		return it->second;
	}
	throw std::out_of_range(fmt::format("Field '{}' does not exist in object {}", name_, this->debug()));
}

void Object::setField(const std::string& name_, ObjectRef value_) {
	monitorCheck();  // Ensure the current thread owns the monitor (if locked)
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
	return (int32_t)(int64_t)_value.load();
}
int64_t NumberObject::getLongValue() const {
	return (int64_t)_value.load();
}
int32_t NumberObject::getAndSet(int32_t newValue) {
	return static_cast<int32_t>(_value.exchange(static_cast<uint64_t>(static_cast<int64_t>(newValue))));
}
int64_t NumberObject::getAndSet(int64_t newValue) {
	return static_cast<int64_t>(_value.exchange(static_cast<uint64_t>(newValue)));
}
int32_t NumberObject::getAndAdd(int32_t delta) {
	return static_cast<int32_t>(_value.fetch_add(static_cast<uint64_t>(static_cast<int64_t>(delta))));
}
int64_t NumberObject::getAndAdd(int64_t delta) {
	return static_cast<int64_t>(_value.fetch_add(static_cast<uint64_t>(delta)));
}
int32_t NumberObject::addAndGet(int32_t delta) {
	return static_cast<int32_t>(_value.fetch_add(static_cast<uint64_t>(static_cast<int64_t>(delta))) + static_cast<int64_t>(delta));
}
int64_t NumberObject::addAndGet(int64_t delta) {
	return static_cast<int64_t>(_value.fetch_add(static_cast<uint64_t>(delta)) + static_cast<uint64_t>(delta));
}
bool NumberObject::compareAndSet(int32_t expect, int32_t update) {
	uint64_t expected = static_cast<uint64_t>(static_cast<int64_t>(expect));
	return _value.compare_exchange_strong(expected, static_cast<uint64_t>(static_cast<int64_t>(update)));
}
bool NumberObject::compareAndSet(int64_t expect, int64_t update) {
	uint64_t expected = static_cast<uint64_t>(expect);
	return _value.compare_exchange_strong(expected, static_cast<uint64_t>(update));
}
bool NumberObject::weakCompareAndSet(int32_t expect, int32_t update) {
	uint64_t expected = static_cast<uint64_t>(static_cast<int64_t>(expect));
	return _value.compare_exchange_weak(expected, static_cast<uint64_t>(static_cast<int64_t>(update)));
}
bool NumberObject::weakCompareAndSet(int64_t expect, int64_t update) {
	uint64_t expected = static_cast<uint64_t>(expect);
	return _value.compare_exchange_weak(expected, static_cast<uint64_t>(update));
}
bool NumberObject::isNumberObject() const {
	return true;
}
std::string NumberObject::debug() const {
	return fmt::format("NumberObject: {:#x}", _value.load());
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
		const auto& field = _class.getField(fieldname);
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
			const auto& field = current->getField(fieldname);
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
void ConstClassObject::monitorEnter() {
	_type.monitorEnter();
}
void ConstClassObject::monitorExit() {
	_type.monitorExit();
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
