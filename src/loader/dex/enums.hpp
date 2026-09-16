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

#ifndef __SANDVIK_DEX_ENUMS_HPP__
#define __SANDVIK_DEX_ENUMS_HPP__

namespace sandvik {
	namespace dex {
		/** @brief DEX access_flags bit values, as defined by the Dalvik executable format spec.
		 * Values match the standard dex access_flags encoding.
		 */
		enum ACCESS_FLAGS {
			ACC_UNKNOWN = 0x0,
			ACC_PUBLIC = 0x1,
			ACC_PRIVATE = 0x2,
			ACC_PROTECTED = 0x4,
			ACC_STATIC = 0x8,
			ACC_FINAL = 0x10,
			ACC_SYNCHRONIZED = 0x20,
			ACC_VOLATILE = 0x40,
			ACC_BRIDGE = 0x40,
			ACC_TRANSIENT = 0x80,
			ACC_VARARGS = 0x80,
			ACC_NATIVE = 0x100,
			ACC_INTERFACE = 0x200,
			ACC_ABSTRACT = 0x400,
			ACC_STRICT = 0x800,
			ACC_SYNTHETIC = 0x1000,
			ACC_ANNOTATION = 0x2000,
			ACC_ENUM = 0x4000,
			ACC_CONSTRUCTOR = 0x10000,
			ACC_DECLARED_SYNCHRONIZED = 0x20000
		};

		/** @brief Type descriptor kind, mirroring how a type_id's descriptor string is classified. */
		enum class TYPE_KIND {
			UNKNOWN = 0,
			PRIMITIVE = 1,
			CLASS = 2,
			ARRAY = 3,
		};

		/** @brief Primitive type descriptor characters. */
		enum class PRIMITIVES {
			VOID_T = 0x01,
			BOOLEAN = 0x02,
			BYTE = 0x03,
			SHORT = 0x04,
			CHAR = 0x05,
			INT = 0x06,
			LONG = 0x07,
			FLOAT = 0x08,
			DOUBLE = 0x09,
		};
	}  // namespace dex
}  // namespace sandvik

#endif  // __SANDVIK_DEX_ENUMS_HPP__
