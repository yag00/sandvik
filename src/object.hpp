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

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace sandvik {
	class ClassLoader;
	class Class;
	class Object;
	class Array;

	using ObjectRef = std::shared_ptr<Object>;
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
			/**
			 * @brief Default constructor.
			 */
			Object() = default;

			/**
			 * @brief Virtual destructor.
			 */
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
			 * @param Unused parameter for nullptr comparison.
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
			 * @brief Returns a debug string representation of the object.
			 * @return Debug string.
			 */
			virtual std::string debug() const;

			/**
			 * @brief Checks if the object is an instance of the specified type.
			 * @param instance_ Name of the type to check.
			 * @return True if the object is an instance of the specified type, false otherwise.
			 */
			virtual bool isInstanceOf(const std::string& instance_) const;

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

			/**
			 * @brief Checks if the object is null.
			 * @return True if the object is null, false otherwise.
			 */
			virtual bool isNull() const;

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

		protected:
			/**
			 * @brief Map storing field names and their corresponding ObjectRef values.
			 */
			std::map<std::string, ObjectRef> _fields;
	};
}  // namespace sandvik

#endif  // __OBJECT_HPP__
