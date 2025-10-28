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

#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>

namespace LIEF {
	namespace DEX {
		class Type;
		class Method;
	}  // namespace DEX
}  // namespace LIEF

namespace sandvik {
	/** @brief Get the type descriptor for a given type.
	 * @param type Type to get the descriptor for.
	 * @return Type descriptor as a string.
	 */
	std::string get_type_descriptor(const LIEF::DEX::Type& type);
	/** @brief Get the type descriptor for a given method.
	 * @param method Method to get the descriptor for.
	 * @return Method descriptor as a string.
	 */
	std::string get_method_descriptor(const LIEF::DEX::Method& method);
	/** @brief Get the primitive type for a given descriptor.
	 * @param descriptor Type descriptor.
	 * @return Primitive type as a string.
	 */
	std::string get_primitive_type(const std::string& descriptor);
	/** @brief Check if the given class name represents a primitive type.
	 * @param classname Class name to check.
	 * @return true if the class name is a primitive type, false otherwise.
	 */
	bool is_primitive_type(const std::string& classname);
}  // namespace sandvik

#endif  // __CLASS_HPP__