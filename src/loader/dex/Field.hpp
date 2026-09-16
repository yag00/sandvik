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

#ifndef __SANDVIK_DEX_FIELD_HPP__
#define __SANDVIK_DEX_FIELD_HPP__

#include <cstddef>
#include <string>
#include <vector>

#include "Annotation.hpp"
#include "Type.hpp"
#include "enums.hpp"

namespace sandvik {
	namespace dex {
		/** @brief A DEX field (encoded_field + its field_id_item), decoded once at class-parse time.
		 */
		class Field {
			public:
				/** @brief List type returned by access_flags(). */
				using access_flags_list_t = std::vector<ACCESS_FLAGS>;

				/** @brief Constructs a fully-decoded field.
				 * @param name_ Field name.
				 * @param type_ Field type.
				 * @param accessFlags_ Combined access_flags bits.
				 * @param index_ Index of this field within the DEX field_ids table.
				 */
				Field(std::string name_, Type type_, uint32_t accessFlags_, size_t index_)
				    : _name(std::move(name_)), _type(std::move(type_)), _accessFlags(accessFlags_), _index(index_) {
				}

				/** @brief Field name. */
				const std::string& name() const {
					return _name;
				}
				/** @brief Field type. */
				const Type* type() const {
					return &_type;
				}
				/** @brief True if this field is static. */
				bool is_static() const {
					return (_accessFlags & ACC_STATIC) != 0;
				}
				/** @brief Checks whether this field has the given access flag set.
				 * @param f_ Flag to test.
				 * @return true if set.
				 */
				bool has(ACCESS_FLAGS f_) const {
					return (_accessFlags & static_cast<uint32_t>(f_)) != 0;
				}
				/** @brief The combined access_flags value, wrapped in a single-element list. */
				access_flags_list_t access_flags() const {
					return {static_cast<ACCESS_FLAGS>(_accessFlags)};
				}
				/** @brief Index of this field within the DEX field_ids table. */
				size_t index() const {
					return _index;
				}
				/** @brief This field's annotations, if any (empty if none, or if this field wasn't
				 * built from a class with an annotations_directory_item). */
				const std::vector<Annotation>& annotations() const {
					return _annotations;
				}
				/** @brief Attaches this field's annotations, decoded from its class's
				 * annotations_directory_item. Called at most once, right after construction, by
				 * File::parseAnnotationsDirectory().
				 * @param annotations_ This field's annotation set. */
				void setAnnotations(std::vector<Annotation> annotations_) {
					_annotations = std::move(annotations_);
				}

			private:
				std::string _name;
				Type _type;
				uint32_t _accessFlags;
				size_t _index;
				std::vector<Annotation> _annotations;
		};
	}  // namespace dex
}  // namespace sandvik

#endif  // __SANDVIK_DEX_FIELD_HPP__
