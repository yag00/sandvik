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

#ifndef __SANDVIK_DEX_CLASS_HPP__
#define __SANDVIK_DEX_CLASS_HPP__

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "Annotation.hpp"
#include "Field.hpp"
#include "Method.hpp"
#include "enums.hpp"
#include "iterator.hpp"

namespace sandvik {
	namespace dex {
		/** @brief A DEX class.
		 */
		class Class {
			public:
				/** @brief List type returned by access_flags(). */
				using access_flags_list_t = std::vector<ACCESS_FLAGS>;

				/** @brief Builds a name-only stand-in Class from just its descriptor: no methods,
				 * fields, access flags, or parent. Every consumer of such a stand-in (Type::cls(),
				 * an external superclass reached via parent()) only ever reads its name.
				 * @param descriptor_ Mangled class descriptor, e.g. "Ljava/lang/String;".
				 */
				explicit Class(std::string descriptor_) : _descriptor(std::move(descriptor_)) {
				}

				/** @brief Builds a fully-decoded Class from a parsed class_def_item/class_data_item.
				 * @param descriptor_ Mangled class descriptor, e.g. "Lcom/example/Foo;".
				 * @param accessFlags_ Combined access_flags bits.
				 * @param superclassDescriptor_ Mangled superclass descriptor, or empty if none.
				 * @param methods_ This class's own methods (direct + virtual).
				 * @param fields_ This class's own fields (static + instance).
				 * @param interfaces_ Pretty (dotted) names of implemented interfaces.
				 * @param annotations_ This class's own annotations (class_annotations), if any.
				 * @param index_ Index of this class_def within the DEX class_defs table.
				 */
				Class(std::string descriptor_, uint32_t accessFlags_, std::string superclassDescriptor_, std::vector<std::unique_ptr<Method>> methods_,
				      std::vector<std::unique_ptr<Field>> fields_, std::vector<std::string> interfaces_, std::vector<Annotation> annotations_, size_t index_)
				    : _descriptor(std::move(descriptor_)),
				      _accessFlags(accessFlags_),
				      _methods(std::move(methods_)),
				      _fields(std::move(fields_)),
				      _interfaces(std::move(interfaces_)),
				      _annotations(std::move(annotations_)),
				      _index(index_) {
					if (!superclassDescriptor_.empty()) {
						_parent = std::make_unique<Class>(std::move(superclassDescriptor_));
					}
				}

				/** @brief Mangled class name (e.g. "Lcom/example/Foo;"). */
				const std::string& fullname() const {
					return _descriptor;
				}
				/** @brief Package name in dotted form (e.g. "com.example"), or empty for the default package. */
				std::string package_name() const;
				/** @brief Simple class name (e.g. "Foo"). */
				std::string name() const;
				/** @brief Demangled class name in dotted form (e.g. "com.example.Foo"). */
				std::string pretty_name() const;

				/** @brief Checks whether this class has the given access flag set.
				 * @param f_ Flag to test.
				 * @return true if set.
				 */
				bool has(ACCESS_FLAGS f_) const {
					return (_accessFlags & static_cast<uint32_t>(f_)) != 0;
				}
				/** @brief See Field::access_flags(): callers only OR this back together. */
				access_flags_list_t access_flags() const {
					return {static_cast<ACCESS_FLAGS>(_accessFlags)};
				}

				/** @brief True if this class extends another one (i.e. is not java.lang.Object). */
				bool has_parent() const {
					return _parent != nullptr;
				}
				/** @brief The superclass, or nullptr if has_parent() is false. */
				const Class* parent() const {
					return _parent.get();
				}
				/** @brief The superclass, or nullptr if has_parent() is false. */
				Class* parent() {
					return _parent.get();
				}

				/** @brief This class's own methods (direct + virtual), not including inherited ones. */
				PtrRange<Method> methods() const {
					return PtrRange<Method>(_methods);
				}
				/** @brief This class's own fields (static + instance), not including inherited ones. */
				PtrRange<Field> fields() const {
					return PtrRange<Field>(_fields);
				}
				/** @brief Pretty (dotted) names of the interfaces this class directly implements. */
				const std::vector<std::string>& interfaces() const {
					return _interfaces;
				}
				/** @brief This class's own annotations (class_annotations), if any. */
				const std::vector<Annotation>& annotations() const {
					return _annotations;
				}

				/** @brief Index of this class_def within the DEX class_defs table. */
				size_t index() const {
					return _index;
				}

			private:
				std::string _descriptor;
				uint32_t _accessFlags = 0;
				std::vector<std::unique_ptr<Method>> _methods;
				std::vector<std::unique_ptr<Field>> _fields;
				std::vector<std::string> _interfaces;
				std::vector<Annotation> _annotations;
				std::unique_ptr<Class> _parent;
				size_t _index = 0;
		};
	}  // namespace dex
}  // namespace sandvik

#endif  // __SANDVIK_DEX_CLASS_HPP__
