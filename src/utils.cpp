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

#include <map>
#include <sstream>

#include "exceptions.hpp"
#include "loader/dex/Class.hpp"
#include "loader/dex/Method.hpp"
#include "loader/dex/Prototype.hpp"
#include "loader/dex/Type.hpp"

using namespace sandvik::dex;

using enum PRIMITIVES;
using enum Type::TYPES;

std::string sandvik::get_type_descriptor(const dex::Type& type) {
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
std::string sandvik::get_method_descriptor(const dex::Method& method) {
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
	static std::map<std::string, std::string, std::less<>> descriptors = {{"Z", "boolean"}, {"B", "byte"},  {"C", "char"},   {"S", "short"}, {"I", "int"},
	                                                                      {"J", "long"},    {"F", "float"}, {"D", "double"}, {"V", "void"}};
	return descriptors[descriptor];
}

bool sandvik::is_primitive_type(const std::string& classname) {
	static std::map<std::string, bool, std::less<>> primitive_types = {{"boolean", true}, {"byte", true},  {"char", true},   {"short", true}, {"int", true},
	                                                                   {"long", true},    {"float", true}, {"double", true}, {"void", true}};
	return primitive_types[classname];
}

std::string sandvik::get_primitive_descriptor(const std::string& type_name) {
	static std::map<std::string, std::string, std::less<>> descriptors = {{"boolean", "Z"}, {"byte", "B"},  {"char", "C"},   {"short", "S"}, {"int", "I"},
	                                                                      {"long", "J"},    {"float", "F"}, {"double", "D"}, {"void", "V"}};
	return descriptors[type_name];
}

std::u16string sandvik::utf8_to_utf16(const std::string& str) {
	std::u16string result;
	result.reserve(str.size());
	const size_t size = str.size();
	for (size_t i = 0; i < size;) {
		const auto c = static_cast<unsigned char>(str[i]);
		auto continuation = [&str, size](size_t pos_) { return pos_ < size && (static_cast<unsigned char>(str[pos_]) & 0xc0) == 0x80; };
		auto bits = [&str](size_t pos_) { return static_cast<uint32_t>(static_cast<unsigned char>(str[pos_]) & 0x3f); };
		if (c < 0x80) {
			result.push_back(c);
			i += 1;
		} else if ((c & 0xe0) == 0xc0 && continuation(i + 1)) {
			result.push_back(static_cast<char16_t>(((c & 0x1f) << 6) | bits(i + 1)));
			i += 2;
		} else if ((c & 0xf0) == 0xe0 && continuation(i + 1) && continuation(i + 2)) {
			// also the CESU-8 halves of a surrogate pair
			result.push_back(static_cast<char16_t>(((c & 0x0f) << 12) | (bits(i + 1) << 6) | bits(i + 2)));
			i += 3;
		} else if ((c & 0xf8) == 0xf0 && continuation(i + 1) && continuation(i + 2) && continuation(i + 3)) {
			const uint32_t cp = (((c & 0x07) << 18) | (bits(i + 1) << 12) | (bits(i + 2) << 6) | bits(i + 3)) - 0x10000;
			result.push_back(static_cast<char16_t>(0xd800 + (cp >> 10)));
			result.push_back(static_cast<char16_t>(0xdc00 + (cp & 0x3ff)));
			i += 4;
		} else {
			result.push_back(c);
			i += 1;
		}
	}
	return result;
}

std::string sandvik::utf16_to_utf8(const std::u16string& str) {
	std::string result;
	result.reserve(str.size());
	for (size_t i = 0; i < str.size(); ++i) {
		uint32_t cp = str[i];
		if (cp >= 0xd800 && cp <= 0xdbff && i + 1 < str.size() && str[i + 1] >= 0xdc00 && str[i + 1] <= 0xdfff) {
			cp = 0x10000 + ((cp - 0xd800) << 10) + (str[++i] - 0xdc00);
		}
		if (cp < 0x80) {
			result.push_back(static_cast<char>(cp));
		} else if (cp < 0x800) {
			result.push_back(static_cast<char>(0xc0 | (cp >> 6)));
			result.push_back(static_cast<char>(0x80 | (cp & 0x3f)));
		} else if (cp < 0x10000) {
			result.push_back(static_cast<char>(0xe0 | (cp >> 12)));
			result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3f)));
			result.push_back(static_cast<char>(0x80 | (cp & 0x3f)));
		} else {
			result.push_back(static_cast<char>(0xf0 | (cp >> 18)));
			result.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3f)));
			result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3f)));
			result.push_back(static_cast<char>(0x80 | (cp & 0x3f)));
		}
	}
	return result;
}
