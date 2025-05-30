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
	std::string get_type_descriptor(const LIEF::DEX::Type& type);
	std::string get_method_descriptor(const LIEF::DEX::Method& method);
}  // namespace sandvik

#endif  // __CLASS_HPP__