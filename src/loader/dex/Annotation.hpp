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

#ifndef __SANDVIK_DEX_ANNOTATION_HPP__
#define __SANDVIK_DEX_ANNOTATION_HPP__

#include <cstdint>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace sandvik {
	namespace dex {
		/** @brief annotation_item visibility byte: which build stage(s) can see the annotation. */
		enum class AnnotationVisibility {
			BUILD = 0,
			RUNTIME = 1,
			SYSTEM = 2,
		};

		struct AnnotationElement;

		/** @brief A single decoded encoded_value (DEX format). Fully resolved at parse time:
		 * string/type/field/method/enum indices are already turned into their resolved names, so
		 * nothing downstream needs the originating File to make sense of a value.
		 */
		class EncodedValue {
			public:
				/** @brief Discriminates which alternative of the encoded_value union is held. */
				enum class Kind {
					Int,         ///< byte/short/char/int/long, widened to int64_t
					Float,       ///< float/double, widened to double
					String,      ///< resolved string (VALUE_STRING)
					Type,        ///< resolved pretty class name (VALUE_TYPE)
					Field,       ///< resolved "Class.name" (VALUE_FIELD)
					Method,      ///< resolved "Class.name" (VALUE_METHOD)
					Enum,        ///< resolved "Class.name" of the enum constant's field (VALUE_ENUM)
					Array,       ///< VALUE_ARRAY
					Annotation,  ///< VALUE_ANNOTATION (a nested encoded_annotation)
					Null,        ///< VALUE_NULL
					Bool,        ///< VALUE_BOOLEAN
				};

				/** @brief Builds an Int-kind value (byte/short/char/int/long, already widened). */
				static EncodedValue makeInt(int64_t value_);
				/** @brief Builds a Float-kind value (float/double, already widened). */
				static EncodedValue makeFloat(double value_);
				/** @brief Builds a String/Type/Field/Method/Enum-kind value from its resolved text.
				 * @param kind_ Must be one of String, Type, Field, Method, Enum.
				 * @param value_ Already-resolved text (e.g. the string itself, or a pretty class name).
				 */
				static EncodedValue makeResolvedString(Kind kind_, std::string value_);
				/** @brief Builds an Array-kind value. */
				static EncodedValue makeArray(std::vector<EncodedValue> items_);
				/** @brief Builds an Annotation-kind value (a nested encoded_annotation). */
				static EncodedValue makeAnnotation(std::string type_, std::vector<AnnotationElement> elements_);
				/** @brief Builds a Null-kind value. */
				static EncodedValue makeNull();
				/** @brief Builds a Bool-kind value. */
				static EncodedValue makeBool(bool value_);

				/** @brief Which alternative this value holds. */
				Kind kind() const {
					return _kind;
				}
				/** @brief Valid when kind() == Int. */
				int64_t asInt() const {
					return std::get<int64_t>(_scalar);
				}
				/** @brief Valid when kind() == Float. */
				double asFloat() const {
					return std::get<double>(_scalar);
				}
				/** @brief Valid when kind() == Bool. */
				bool asBool() const {
					return std::get<bool>(_scalar);
				}
				/** @brief Valid when kind() is String, Type, Field, Method, or Enum. */
				const std::string& asString() const {
					return std::get<std::string>(_scalar);
				}
				/** @brief Valid when kind() == Array. */
				const std::vector<EncodedValue>& asArray() const {
					return std::get<std::vector<EncodedValue>>(_scalar);
				}
				/** @brief Valid when kind() == Annotation: the nested annotation's type (pretty name). */
				const std::string& annotationType() const {
					return _annotationType;
				}
				/** @brief Valid when kind() == Annotation: the nested annotation's name/value pairs. */
				const std::vector<AnnotationElement>& annotationElements() const {
					return _annotationElements;
				}

			private:
				EncodedValue() = default;  // built only via the make*() factories above

				Kind _kind = Kind::Null;
				std::variant<std::monostate, int64_t, double, bool, std::string, std::vector<EncodedValue>> _scalar;
				// Only meaningful when _kind == Annotation (kept out of _scalar since a nested
				// annotation needs two payloads - a type name and an element list - not one).
				std::string _annotationType;
				std::vector<AnnotationElement> _annotationElements;
		};

		/** @brief One {name, value} pair inside an encoded_annotation. */
		struct AnnotationElement {
				std::string name;    ///< Element name (e.g. "value", "accessFlags").
				EncodedValue value;  ///< Element value.
		};

		/** @brief A single DEX annotation instance: an annotation_item's visibility byte plus its
		 * encoded_annotation payload (type + name/value pairs).
		 */
		class Annotation {
			public:
				/** @brief Constructs a fully-decoded annotation.
				 * @param type_ Pretty (dotted) name of the annotation type, e.g. "dalvik.annotation.InnerClass".
				 * @param visibility_ Build stage(s) this annotation is visible to.
				 * @param elements_ This annotation's name/value pairs.
				 */
				Annotation(std::string type_, AnnotationVisibility visibility_, std::vector<AnnotationElement> elements_)
				    : _type(std::move(type_)), _visibility(visibility_), _elements(std::move(elements_)) {
				}

				/** @brief Pretty (dotted) name of the annotation type, e.g. "dalvik.annotation.InnerClass". */
				const std::string& type() const {
					return _type;
				}
				/** @brief Build stage(s) this annotation is visible to. */
				AnnotationVisibility visibility() const {
					return _visibility;
				}
				/** @brief This annotation's name/value pairs. */
				const std::vector<AnnotationElement>& elements() const {
					return _elements;
				}
				/** @brief Looks up an element by name.
				 * @param name_ Element name.
				 * @return Pointer to the value, or nullptr if this annotation has no such element.
				 */
				const EncodedValue* element(const std::string& name_) const {
					for (const auto& e : _elements) {
						if (e.name == name_) {
							return &e.value;
						}
					}
					return nullptr;
				}

			private:
				std::string _type;
				AnnotationVisibility _visibility;
				std::vector<AnnotationElement> _elements;
		};

		inline EncodedValue EncodedValue::makeInt(int64_t value_) {
			EncodedValue v;
			v._kind = Kind::Int;
			v._scalar = value_;
			return v;
		}
		inline EncodedValue EncodedValue::makeFloat(double value_) {
			EncodedValue v;
			v._kind = Kind::Float;
			v._scalar = value_;
			return v;
		}
		inline EncodedValue EncodedValue::makeResolvedString(Kind kind_, std::string value_) {
			EncodedValue v;
			v._kind = kind_;
			v._scalar = std::move(value_);
			return v;
		}
		inline EncodedValue EncodedValue::makeArray(std::vector<EncodedValue> items_) {
			EncodedValue v;
			v._kind = Kind::Array;
			v._scalar = std::move(items_);
			return v;
		}
		inline EncodedValue EncodedValue::makeAnnotation(std::string type_, std::vector<AnnotationElement> elements_) {
			EncodedValue v;
			v._kind = Kind::Annotation;
			v._annotationType = std::move(type_);
			v._annotationElements = std::move(elements_);
			return v;
		}
		inline EncodedValue EncodedValue::makeNull() {
			EncodedValue v;
			v._kind = Kind::Null;
			return v;
		}
		inline EncodedValue EncodedValue::makeBool(bool value_) {
			EncodedValue v;
			v._kind = Kind::Bool;
			v._scalar = value_;
			return v;
		}
	}  // namespace dex
}  // namespace sandvik

#endif  // __SANDVIK_DEX_ANNOTATION_HPP__
