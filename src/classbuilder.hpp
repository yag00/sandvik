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

#ifndef __CLASSBUILDER_HPP__
#define __CLASSBUILDER_HPP__

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "object.hpp"

namespace sandvik {
	class ClassLoader;
	class Frame;
	class Class;
	/**
	 * @class ClassBuilder
	 * @brief Helper class to build Class objects dynamically.
	 *
	 * The ClassBuilder class provides methods to define methods, fields, and superclass
	 * for a Class object. It allows adding virtual methods, regular methods, and fields,
	 * as well as setting the superclass and interfaces. Once all components are added,
	 * the finalize method is called to complete the class definition.
	 */
	class ClassBuilder {
		public:
			/** @brief Constructs a ClassBuilder
			 * @param classLoader Reference to the ClassLoader
			 * @param packageName Package name of the class
			 * @param className Name of the class
			 */
			ClassBuilder(ClassLoader& classLoader, const std::string& packageName, const std::string& className);

			/** @brief Adds a virtual method to the class
			 * @param name_ Name of the method
			 * @param signature_ Signature of the method
			 * @param flags_ Method flags
			 * @param function_ Function implementing the method
			 */
			void addVirtualMethod(const std::string& name_, const std::string& signature_, uint64_t flags_,
			                      std::function<void(Frame&, std::vector<ObjectRef>&)> function_);
			/** @brief Adds a regular method to the class
			 * @param name_ Name of the method
			 * @param signature_ Signature of the method
			 * @param flags_ Method flags
			 * @param function_ Function implementing the method
			 */
			void addMethod(const std::string& name_, const std::string& signature_, uint64_t flags_,
			               std::function<void(Frame&, std::vector<ObjectRef>&)> function_);

			/** @brief Adds a field to the class
			 * @param name_ Name of the field
			 * @param type_ Type of the field
			 * @param isStatic_ Whether the field is static
			 * @param value_ Initial value of the field
			 */
			void addField(const std::string& name_, const std::string& type_, bool isStatic_, ObjectRef value_ = nullptr);

			/** @brief Sets the superclass of the class
			 * @param superClassName_ Name of the superclass
			 */
			void setSuperClass(const std::string& superClassName_);
			/** @brief Sets the class as an interface */
			void setInterface();
			/** @brief Finalizes the class definition */
			void finalize();

		private:
			ClassLoader& _classLoader;
			std::unique_ptr<Class> _class;
			uint32_t _methodIndex = 1;
	};

}  // namespace sandvik

#endif  // __CLASSBUILDER_HPP__