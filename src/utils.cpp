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

#include <LIEF/DEX/Class.hpp>
#include <LIEF/DEX/Method.hpp>
#include <LIEF/DEX/Prototype.hpp>
#include <LIEF/DEX/Type.hpp>
#include <map>
#include <sstream>

#include "exceptions.hpp"

using namespace LIEF::DEX;

using enum Type::PRIMITIVES;
using enum Type::TYPES;

std::string sandvik::get_type_descriptor(const Type& type) {
	switch (type.type()) {
		case PRIMITIVE:
			switch (type.primitive()) {
				case VOID_T:
					return "V";
				case BOOLEAN:
					return "Z";
				case BYTE:
					return "B";
				case SHORT:
					return "S";
				case CHAR:
					return "C";
				case INT:
					return "I";
				case LONG:
					return "J";
				case FLOAT:
					return "F";
				case DOUBLE:
					return "D";
				default:
					throw VmException("Unknown primitive type");
			}

		case CLASS: {
			const auto& cls = type.cls();
			return cls.fullname();  // e.g., Ljava/lang/String;
		}

		case ARRAY: {
			std::string t = "[";
			for (const auto& elem : type.array()) {
				t += get_type_descriptor(elem);
			}
			return t;
		}

		default:
			throw VmException("Unknown or unsupported type {}", (int)type.type());
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

std::string sandvik::get_primitive_type(const std::string& descriptor) {
	static std::map<std::string, std::string> descriptors = {{"Z", "boolean"}, {"B", "byte"},  {"C", "char"},   {"S", "short"}, {"I", "int"},
	                                                         {"J", "long"},    {"F", "float"}, {"D", "double"}, {"V", "void"}};
	return descriptors[descriptor];
}

bool sandvik::is_primitive_type(const std::string& classname) {
	static std::map<std::string, bool> primitive_types = {{"boolean", true}, {"byte", true},  {"char", true},   {"short", true}, {"int", true},
	                                                      {"long", true},    {"float", true}, {"double", true}, {"void", true}};
	return primitive_types[classname];
}
