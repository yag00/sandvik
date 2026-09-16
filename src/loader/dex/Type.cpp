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

#include "Type.hpp"

#include "Class.hpp"
#include "reader.hpp"

using namespace sandvik::dex;

Type::Type(std::string descriptor_) : _descriptor(std::move(descriptor_)) {
	if (_descriptor.empty()) {
		throw DexFormatException("Empty type descriptor");
	}
	switch (_descriptor[0]) {
		case 'V':
			_kind = TYPES::PRIMITIVE;
			_primitive = PRIMITIVES::VOID_T;
			break;
		case 'Z':
			_kind = TYPES::PRIMITIVE;
			_primitive = PRIMITIVES::BOOLEAN;
			break;
		case 'B':
			_kind = TYPES::PRIMITIVE;
			_primitive = PRIMITIVES::BYTE;
			break;
		case 'S':
			_kind = TYPES::PRIMITIVE;
			_primitive = PRIMITIVES::SHORT;
			break;
		case 'C':
			_kind = TYPES::PRIMITIVE;
			_primitive = PRIMITIVES::CHAR;
			break;
		case 'I':
			_kind = TYPES::PRIMITIVE;
			_primitive = PRIMITIVES::INT;
			break;
		case 'J':
			_kind = TYPES::PRIMITIVE;
			_primitive = PRIMITIVES::LONG;
			break;
		case 'F':
			_kind = TYPES::PRIMITIVE;
			_primitive = PRIMITIVES::FLOAT;
			break;
		case 'D':
			_kind = TYPES::PRIMITIVE;
			_primitive = PRIMITIVES::DOUBLE;
			break;
		case 'L':
			_kind = TYPES::CLASS;
			break;
		case '[':
			_kind = TYPES::ARRAY;
			_arrayElem.emplace_back(_descriptor.substr(1));
			break;
		default:
			throw DexFormatException("Unknown type descriptor: " + _descriptor);
	}
}

Class Type::cls() const {
	return Class(_descriptor);
}
