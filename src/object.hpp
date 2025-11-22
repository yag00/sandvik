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

#ifndef __OBJECT_HPP__
#define __OBJECT_HPP__

#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace sandvik {
	class ClassLoader;
	class Class;
	class Object;
	class Array;
	class Monitor;

	/** @brief Object reference type */
	using ObjectRef = Object*;
	/**
	 * @class Object
	 * @brief Base class representing a generic java object.
	 *
	 * The Object class provides an interface for objects that can store fields, support type checking,
	 * and represent various object types such as numbers, arrays, and null objects. Copy and move operations
	 * are explicitly disabled to ensure unique object identity.
	 */
	class Object {
		public:
			Object();
			virtual ~Object() = default;

			/**
			 * @brief Deleted copy constructor.
			 */
			Object(const Object&) = delete;

			/**
			 * @brief Deleted copy assignment operator.
			 */
			Object& operator=(const Object&) = delete;

			/**
			 * @brief Deleted move constructor.
			 */
			Object(Object&& other) noexcept = delete;

			/**
			 * @brief Deleted move assignment operator.
			 */
			Object& operator=(Object&& other) noexcept = delete;

			/**
			 * @brief Equality operator for comparing with another Object.
			 * @param other Reference to the other Object.
			 * @return True if objects are equal, false otherwise.
			 */
			virtual bool operator==(const Object& other) const;

			/**
			 * @brief Equality operator for comparing with nullptr.
			 * @return True if the object is null, false otherwise.
			 */
			virtual bool operator==(std::nullptr_t) const;

			/**
			 * @brief Creates an ObjectRef from a Class instance.
			 * @param class_ Reference to the Class object.
			 * @return ObjectRef to the created object.
			 */
			static ObjectRef make(Class& class_);

			/**
			 * @brief Creates an ObjectRef representing a number object.
			 * @param number_ The numeric value.
			 * @return ObjectRef to the created number object.
			 */
			static ObjectRef make(uint64_t number_);

			/**
			 * @brief Creates an ObjectRef from a class loader and string.
			 * @param classloader_ Reference to the ClassLoader.
			 * @param str_ String value to initialize the object.
			 * @return ObjectRef to the created object.
			 */
			static ObjectRef make(ClassLoader& classloader_, const std::string& str_);

			/**
			 * @brief Creates a constant class ObjectRef for Class<?> objects.
			 * @param classloader_ Reference to the ClassLoader.
			 * @param classtype_ Reference to the Class type.
			 * @return ObjectRef to the constant class object.
			 */
			static ObjectRef makeConstClass(ClassLoader& classloader_, Class& classtype_);

			/**
			 * @brief Creates a null ObjectRef.
			 * @return ObjectRef representing a null object.
			 */
			static ObjectRef makeNull();

			/**
			 * @brief Creates an array ObjectRef with specified dimensions.
			 * @param classloader_ Reference to the ClassLoader.
			 * @param classtype_ Reference to the Class type of the array.
			 * @param dimensions_ Vector of dimensions for the array.
			 * @return ObjectRef to the created array object.
			 */
			static ObjectRef makeArray(ClassLoader& classloader_, const Class& classtype_, const std::vector<uint32_t>& dimensions_);

			/**
			 * @brief Returns a string representation of the object.
			 * @return string.
			 */
			virtual std::string toString() const;

			/**
			 * @brief Returns the identity hash code for this object.
			 *
			 * The identity hash code is intended to provide a unique identifier for the object instance,
			 * similar to Java's System.identityHashCode.
			 *
			 * @return The identity hash code as a std::size_t value.
			 */
			virtual int32_t identityHashCode() const;

			/**
			 * @brief Checks if the object is an instance of the specified type.
			 * @param instance_ Name of the type to check.
			 * @return True if the object is an instance of the specified type, false otherwise.
			 */
			virtual bool isInstanceOf(const std::string& instance_) const;

			/** @brief Clones the object.
			 * @return Shared pointer to the cloned Object.
			 */
			virtual ObjectRef clone() const;

			/**
			 * @name Number object methods.
			 */
			///@{

			/**
			 * @brief Checks if the object represents a number.
			 * @return True if the object is a number, false otherwise.
			 */
			virtual bool isNumberObject() const;

			/**
			 * @brief Gets the integer value of the object.
			 * @return Integer value.
			 * @throw std::bad_cast if the object is not a number.
			 */
			virtual int32_t getValue() const;

			/**
			 * @brief Gets the long integer value of the object.
			 * @return Long integer value.
			 * @throw std::bad_cast if the object is not a number.
			 */
			virtual int64_t getLongValue() const;

			/**
			 * @brief Atomically sets to the given value and returns the old value.
			 *
			 * @param newValue the new value
			 * @return the previous value
			 */
			virtual int32_t getAndSet(int32_t newValue);
			/**
			 * @brief Atomically sets to the given value and returns the old value.
			 *
			 * @param newValue the new value
			 * @return the previous value
			 */
			virtual int64_t getAndSet(int64_t newValue);
			/**
			 * @brief Atomically adds the given value to the current value.
			 *
			 * @param delta the value to add
			 * @return the previous value
			 */
			virtual int32_t getAndAdd(int32_t delta);
			/**
			 * @brief Atomically adds the given value to the current value.
			 *
			 * @param delta the value to add
			 * @return the previous value
			 */
			virtual int64_t getAndAdd(int64_t delta);
			/**
			 * @brief Atomically adds the given value to the current value.
			 *
			 * @param delta the value to add
			 * @return the updated value
			 */
			virtual int32_t addAndGet(int32_t delta);
			/**
			 * @brief Atomically adds the given value to the current value.
			 *
			 * @param delta the value to add
			 * @return the updated value
			 */
			virtual int64_t addAndGet(int64_t delta);
			/**
			 * @brief Atomically sets the value to the given updated value
			 * if the current value == the expected value.
			 *
			 * @param expect the expected value
			 * @param update the new value
			 * @return true if successful. False return indicates that
			 * the actual value was not equal to the expected value.
			 */
			virtual bool compareAndSet(int32_t expect, int32_t update);
			/**
			 * @brief Atomically sets the value to the given updated value
			 * if the current value == the expected value.
			 *
			 * @param expect the expected value
			 * @param update the new value
			 * @return true if successful. False return indicates that
			 * the actual value was not equal to the expected value.
			 */
			virtual bool compareAndSet(int64_t expect, int64_t update);
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
			virtual bool weakCompareAndSet(int32_t expect, int32_t update);
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
			virtual bool weakCompareAndSet(int64_t expect, int64_t update);

			///@}

			/**
			 * @name Array object methods.
			 */
			///@{

			/**
			 * @brief Checks if the object is an array.
			 * @return True if the object is an array, false otherwise.
			 */
			virtual bool isArray() const;
			/**
			 * @brief Gets the length of the array if the object is an array.
			 * @return Array length.
			 * @throw std::bad_cast if the object is not an array.
			 */
			virtual uint32_t getArrayLength() const;

			///@}

			/**
			 * @brief Checks if the object is null.
			 * @return True if the object is null, false otherwise.
			 */
			virtual bool isNull() const;

			/**
			 * @name Class/Class<?> object methods.
			 */
			///@{
			/**
			 * @brief Checks if the object is a class object (instance of java.lang.Class).
			 * @return True if the object is a class object, false otherwise.
			 * @throw std::bad_cast if object is not a class.
			 */
			virtual bool isClass() const;
			/**
			 * @brief Gets the Class of the object.
			 * @return Reference to the Class object.
			 * @throw std::bad_cast if the object does not have a class.
			 */
			virtual Class& getClass() const;
			/**
			 * @brief Gets the Class type of the object (used by Array or Class<?>).
			 * @return Reference to the Class type.
			 * @throw std::bad_cast if the object do not use this.
			 */
			virtual const Class& getClassType() const;

			/**
			 * @brief Sets the value of a field.
			 * @param name_ Name of the field.
			 * @param value_ ObjectRef to set as the field value.
			 */
			void setField(const std::string& name_, ObjectRef value_);

			/**
			 * @brief Gets the value of a field.
			 * @param name_ Name of the field.
			 * @return ObjectRef of the field value.
			 */
			ObjectRef getField(const std::string& name_) const;

			///@}

			/**
			 * @name String object methods.
			 */
			///@{
			/**
			 * @brief  Checks if the object is a string.
			 * @return True if the object is a string, false otherwise.
			 */
			virtual bool isString() const;
			/**
			 * @brief Gets the string value of the object.
			 * @return String value.
			 * @throw std::bad_cast if the object is not a string.
			 */
			virtual std::string str() const;
			/**
			 * @brief Set the string value of the object.
			 * @param str_ String value.
			 * @throw std::bad_cast if the object is not a string.
			 */
			virtual void setString(const std::string& str_);
			///@}

			/**
			 * @name Thread synchronization methods.
			 */
			///@{
			/** @brief Enter monitor */
			virtual void monitorEnter();
			/** @brief Exit monitor */
			virtual void monitorExit();

			/** @brief Causes the current thread to wait until either another thread invokes the notify methods
			 * @param timeout_ the maximum time to wait in milliseconds.
			 */
			void wait(uint64_t timeout_ = 0);
			/** @brief Wakes up a single thread that is waiting on this object's monitor.
			 * Wakes up a single thread that is waiting on this object's
			 * monitor. If any threads are waiting on this object, one of them
			 * is chosen to be awakened. The choice is arbitrary and occurs at
			 * the discretion of the implementation. */
			void notify();
			/** @brief Wakes up all threads that are waiting on this object's monitor.
			 * Wakes up all threads that are waiting on this object's monitor. A
			 * thread waits on an object's monitor by calling one of the
			 * wait method. */
			void notifyAll();
			///@}

		protected:
			/** Check monitor ownership */
			void monitorCheck() const;
			/** Map storing field names and their corresponding values. */
			std::map<std::string, ObjectRef, std::less<>> _fields;
			/** Monitor for thread synchronization */
			std::unique_ptr<Monitor> _monitor;
	};
}  // namespace sandvik

#endif  // __OBJECT_HPP__
