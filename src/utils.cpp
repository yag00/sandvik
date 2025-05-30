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

#include "utils.hpp"

#include <fmt/format.h>

#include <LIEF/DEX/Class.hpp>
#include <LIEF/DEX/Method.hpp>
#include <LIEF/DEX/Prototype.hpp>
#include <LIEF/DEX/Type.hpp>
#include <sstream>

using namespace LIEF::DEX;

std::string sandvik::get_type_descriptor(const Type& type) {
	switch (type.type()) {
		case Type::TYPES::PRIMITIVE:
			switch (type.primitive()) {
				case Type::PRIMITIVES::VOID_T:
					return "V";
				case Type::PRIMITIVES::BOOLEAN:
					return "Z";
				case Type::PRIMITIVES::BYTE:
					return "B";
				case Type::PRIMITIVES::SHORT:
					return "S";
				case Type::PRIMITIVES::CHAR:
					return "C";
				case Type::PRIMITIVES::INT:
					return "I";
				case Type::PRIMITIVES::LONG:
					return "J";
				case Type::PRIMITIVES::FLOAT:
					return "F";
				case Type::PRIMITIVES::DOUBLE:
					return "D";
				default:
					throw std::runtime_error("Unknown primitive type");
			}

		case Type::TYPES::CLASS: {
			const auto& cls = type.cls();
			return cls.fullname();  // e.g., Ljava/lang/String;
		}

		case Type::TYPES::ARRAY: {
			std::string t = "[";
			for (const auto& elem : type.array()) {
				t += get_type_descriptor(elem);
			}
			return t;
		}

		default:
			throw std::runtime_error(fmt::format("Unknown or unsupported type {}", (int)type.type()));
	}
}
std::string sandvik::get_method_descriptor(const Method& method) {
	std::ostringstream oss;
	oss << '(';
	for (const auto& param : method.prototype()->parameters_type()) {
		oss << get_type_descriptor(param);
	}
	oss << ')';
	oss << get_type_descriptor(*method.prototype()->return_type());
	return oss.str();
}
