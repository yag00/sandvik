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

#ifndef __SANDVIK_DEX_METHOD_HPP__
#define __SANDVIK_DEX_METHOD_HPP__

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "Annotation.hpp"
#include "CodeInfo.hpp"
#include "Prototype.hpp"
#include "enums.hpp"

namespace sandvik {
	namespace dex {
		/** @brief A DEX method (encoded_method + its method_id_item/proto_id_item), decoded once at
		 * class-parse time.
		 */
		class Method {
			public:
				/** @brief List type returned by access_flags(). */
				using access_flags_list_t = std::vector<ACCESS_FLAGS>;
				/** @brief Raw Dalvik bytecode, as bytes. */
				using bytecode_t = std::vector<uint8_t>;

				/** @brief Constructs a fully-decoded method.
				 * @param name_ Method name.
				 * @param prototype_ Method prototype (return + parameter types).
				 * @param accessFlags_ Combined access_flags bits.
				 * @param isVirtual_ True if this method came from the virtual_methods list
				 *   (as opposed to direct_methods) of its class_data_item.
				 * @param index_ Index of this method within the DEX method_ids table.
				 * @param bytecode_ Raw Dalvik bytecode, or empty for an abstract/native method.
				 * @param codeInfo_ Decoded register count and try/catch table.
				 */
				Method(std::string name_, Prototype prototype_, uint32_t accessFlags_, bool isVirtual_, size_t index_, bytecode_t bytecode_, CodeInfo codeInfo_)
				    : _name(std::move(name_)),
				      _prototype(std::move(prototype_)),
				      _accessFlags(accessFlags_),
				      _isVirtual(isVirtual_),
				      _index(index_),
				      _bytecode(std::move(bytecode_)),
				      _codeInfo(std::move(codeInfo_)) {
				}

				/** @brief Method name. */
				const std::string& name() const {
					return _name;
				}
				/** @brief Raw Dalvik bytecode, or empty for an abstract/native method. */
				const bytecode_t& bytecode() const {
					return _bytecode;
				}
				/** @brief Index of this method within the DEX method_ids table. */
				size_t index() const {
					return _index;
				}
				/** @brief True if this method came from the virtual_methods list (not static,
				 * private, or a constructor). */
				bool is_virtual() const {
					return _isVirtual;
				}
				/** @brief This method's prototype (return + parameter types). */
				const Prototype* prototype() const {
					return &_prototype;
				}
				/** @brief Checks whether this method has the given access flag set.
				 * @param f_ Flag to test.
				 * @return true if set.
				 */
				bool has(ACCESS_FLAGS f_) const {
					return (_accessFlags & static_cast<uint32_t>(f_)) != 0;
				}
				/** @brief See Field::access_flags(): every caller only ORs this back together, so a
				 * single combined-value element is bit-for-bit equivalent to a full decomposition. */
				access_flags_list_t access_flags() const {
					return {static_cast<ACCESS_FLAGS>(_accessFlags)};
				}
				/** @brief Decoded register count and try/catch table. */
				const CodeInfo& code_info() const {
					return _codeInfo;
				}
				/** @brief This method's own annotations, if any (empty if none, or if this method
				 * wasn't built from a class with an annotations_directory_item). */
				const std::vector<Annotation>& annotations() const {
					return _annotations;
				}
				/** @brief Attaches this method's annotations, decoded from its class's
				 * annotations_directory_item. Called at most once, right after construction, by
				 * File::parseAnnotationsDirectory().
				 * @param annotations_ This method's annotation set. */
				void setAnnotations(std::vector<Annotation> annotations_) {
					_annotations = std::move(annotations_);
				}
				/** @brief Annotations attached to one formal parameter, if any.
				 * @param paramIdx_ Parameter index.
				 * @return That parameter's annotation set, or an empty set if paramIdx_ is out of
				 * range or has no annotations. */
				const std::vector<Annotation>& parameterAnnotations(size_t paramIdx_) const {
					static const std::vector<Annotation> empty;
					return paramIdx_ < _parameterAnnotations.size() ? _parameterAnnotations[paramIdx_] : empty;
				}
				/** @brief Attaches this method's per-parameter annotations, decoded from its class's
				 * annotations_directory_item. Called at most once, right after construction, by
				 * File::parseAnnotationsDirectory().
				 * @param parameterAnnotations_ One annotation set per formal parameter, in order. */
				void setParameterAnnotations(std::vector<std::vector<Annotation>> parameterAnnotations_) {
					_parameterAnnotations = std::move(parameterAnnotations_);
				}

			private:
				std::string _name;
				Prototype _prototype;
				uint32_t _accessFlags;
				bool _isVirtual;
				size_t _index;
				bytecode_t _bytecode;
				CodeInfo _codeInfo;
				std::vector<Annotation> _annotations;
				std::vector<std::vector<Annotation>> _parameterAnnotations;
		};
	}  // namespace dex
}  // namespace sandvik

#endif  // __SANDVIK_DEX_METHOD_HPP__
