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

#ifndef __CLASS_HPP__
#define __CLASS_HPP__

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "object.hpp"

namespace LIEF::DEX {
	class Class;
}  // namespace LIEF::DEX

namespace sandvik {
	class ClassLoader;
	class Monitor;
	class Method;
	class Field;
	/** @brief Represents a Java class. */
	class Class {
		public:
			/** @brief Constructs a Class object.
			 * @param classloader_ Reference to the ClassLoader
			 * @param packagename_ Package name of the class
			 * @param fullname_ Full name of the class
			 */
			Class(ClassLoader& classloader_, const std::string& packagename_, const std::string& fullname_);
			/** @brief Constructs a Class object from a LIEF DEX Class.
			 * @param classloader_ Reference to the ClassLoader
			 * @param dexIdx_ Index of the DEX file
			 * @param class_ Reference to the LIEF DEX Class
			 */
			Class(ClassLoader& classloader_, const uint32_t dexIdx_, const LIEF::DEX::Class& class_);
			virtual ~Class();

			/** @brief Prints debug information about the class. */
			void debug() const;

			/** @brief Checks if the class is statically initialized.
			 * @return true if the class is statically initialized, false otherwise.
			 */
			bool isStaticInitialized();
			/** @brief Sets the class as statically initialized. */
			void setStaticInitialized();

			/** @brief Gets the DEX index of the class.
			 * @return DEX index.
			 */
			uint32_t getDexIdx() const;
			/** @brief Gets the class name.
			 * @return Class name.
			 */
			std::string getName() const;
			/** @brief Gets the full name of the class.
			 * @return Full name of the class.
			 */
			std::string getFullname() const;

			/** @brief Checks if a class method is overloaded.
			 * @param name_ Name of the method.
			 * @return true if the method is overloaded, false otherwise.
			 */
			bool isMethodOverloaded(const std::string& name_) const;
			/** @brief Checks if a class method exists.
			 * @param name_ Name of the method.
			 * @param descriptor_ Descriptor of the method.
			 * @return true if the method exists, false otherwise.
			 */
			bool hasMethod(const std::string& name_, const std::string& descriptor_) const;
			/** @brief Checks if a class method exists.
			 * @param idx_ Index of the method.
			 * @return true if the method exists, false otherwise.
			 */
			bool hasMethod(uint32_t idx_) const;
			/** @brief Gets a class method by name and descriptor.
			 * @param name_ Name of the method.
			 * @param descriptor_ Descriptor of the method.
			 * @return Reference to the Method object.
			 */
			Method& getMethod(const std::string& name_, const std::string& descriptor_);
			/** @brief Gets a class method by index.
			 * @param idx_ Index of the method.
			 * @return Reference to the Method object.
			 */
			Method& getMethod(uint32_t idx_);

			/** @brief Checks if a class field exists.
			 * @param name_ Name of the field.
			 * @return true if the field exists, false otherwise.
			 */
			bool hasField(const std::string& name_) const;
			/** @brief Checks if a class field exists.
			 * @param idx_ Index of the field.
			 * @return true if the field exists, false otherwise.
			 */
			bool hasField(uint32_t idx_) const;
			/** @brief Gets a class field by name.
			 * @param name_ Name of the field.
			 * @return Reference to the Field object.
			 */
			Field& getField(const std::string& name_);
			/** @brief Gets a class field by index.
			 * @param idx_ Index of the field.
			 * @return Reference to the Field object.
			 */
			Field& getField(uint32_t idx_);
			/** @brief Gets the list of field names.
			 * @return Vector of field names.
			 */
			std::vector<std::string> getFieldList() const;

			/** @brief Checks if a class implements an interface.
			 * @param interface_ Name of the interface.
			 * @return true if the class implements the interface, false otherwise.
			 */
			bool implements(const std::string& interface_) const;
			/** @brief Checks if a class implements an interface.
			 * @param interface_ Reference to the interface Class.
			 * @return true if the class implements the interface, false otherwise.
			 */
			bool implements(const Class& interface_) const;
			/** @brief Checks if the class is an instance of another class.
			 * @param classname_ Name of a class.
			 * @return true if the class is an instance of the specified class, false otherwise.
			 */
			bool isInstanceOf(const std::string& classname_) const;
			/** @brief Checks if the class is an instance of another class.
			 * @param class_ Reference to a Class.
			 * @return true if the class is an instance of the specified class, false otherwise.
			 */
			bool isInstanceOf(const Class& class_) const;
			/** @brief Checks if the class is an instance of another class.
			 * @param class_ a Java object.
			 * @return true if the class is an instance of the specified class, false otherwise.
			 */
			bool isInstanceOf(const ObjectRef class_) const;
			/** @brief Checks if the class is external (declared but defined outside the DEX files).
			 * @return true if the class is external, false otherwise.
			 */
			bool isExternal() const;
			/** @brief Checks if the class is abstract.
			 * @return true if the class is abstract, false otherwise.
			 */
			bool isAbstract() const;
			/** @brief Checks if the class is an interface.
			 * @return true if the class is an interface, false otherwise.
			 */
			bool isInterface() const;
			/** @brief Checks if the class has a superclass.
			 * @return true if the class has a superclass, false otherwise.
			 */
			bool hasSuperClass() const;
			/** @brief Gets the superclass of the class.
			 * @return Reference to the superclass.
			 */
			Class& getSuperClass() const;
			/** @brief Gets the superclass name of the class.
			 * @return Superclass name.
			 */
			std::string getSuperClassname() const;

			/** @brief Enters the monitor.
			 *
			 * used for synchronization of static fields
			 */
			void monitorEnter();
			/** @brief Exits the monitor.
			 *
			 * used for synchronization of static fields
			 */
			void monitorExit();
			/** @brief Checks the monitor state.
			 *
			 * used for synchronization of static fields
			 */
			void monitorCheck() const;

		private:
			ClassLoader& _classloader;
			bool _isStaticInitialized = false;

			std::string _packagename;
			std::string _fullname;
			std::string _name;
			uint32_t _dexIdx;

			bool _isInterface;
			bool _isAbstract;
			bool _hasSuperClass;
			std::string _superClassname;

			std::map<std::string, std::unique_ptr<Method>, std::less<>> _methods;
			std::map<std::string, std::unique_ptr<Field>, std::less<>> _fields;
			std::vector<std::string> _interfaces;
			friend class ClassBuilder;

			std::unique_ptr<Monitor> _monitor;
	};
}  // namespace sandvik

#endif  // __CLASS_HPP__