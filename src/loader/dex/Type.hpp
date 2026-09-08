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

#ifndef __SANDVIK_DEX_TYPE_HPP__
#define __SANDVIK_DEX_TYPE_HPP__

#include <string>
#include <vector>

#include "enums.hpp"

namespace sandvik {
	namespace dex {
		class Class;

		/** @brief Represents a DEX type descriptor (primitive, class, or array), decoded directly
		 * from its descriptor string (e.g. "I", "Ljava/lang/String;", "[[I").
		 */
		class Type {
			public:
				/** @brief Type descriptor kind: primitive, class, or array. */
				using TYPES = TYPE_KIND;

				/** @brief Parses a type descriptor string into its kind (primitive/class/array).
				 * @param descriptor_ Type descriptor, e.g. "I", "Ljava/lang/String;", "[[I".
				 */
				explicit Type(std::string descriptor_);
				/** @brief Copy-constructible: Type is a plain value type. */
				Type(const Type&) = default;
				/** @brief Copy-assignable: Type is a plain value type. */
				Type& operator=(const Type&) = default;

				/** @brief The descriptor this Type was parsed from. */
				const std::string& descriptor() const {
					return _descriptor;
				}

				/** @brief Kind of this type: primitive, class, or array. */
				TYPES type() const {
					return _kind;
				}

				/** @brief Valid when type() == PRIMITIVE. */
				PRIMITIVES primitive() const {
					return _primitive;
				}

				/** @brief Valid when type() == CLASS: a name-only Class view over this type's
				 * descriptor (no methods/fields resolved - every caller only ever reads its name).
				 * Returned by value
				 */
				Class cls() const;

				/** @brief Valid when type() == ARRAY: a single-element view containing the element
				 * type one array dimension down (e.g. for "[[I", the one element describes "[I").
				 */
				const std::vector<Type>& array() const {
					return _arrayElem;
				}

			private:
				std::string _descriptor;
				TYPES _kind = TYPES::UNKNOWN;
				PRIMITIVES _primitive = PRIMITIVES::VOID_T;
				std::vector<Type> _arrayElem;
		};
	}  // namespace dex
}  // namespace sandvik

#endif  // __SANDVIK_DEX_TYPE_HPP__
