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

#ifndef __FIELD_HPP__
#define __FIELD_HPP__

#include <memory>
#include <string>
#include <vector>

#include "object.hpp"

namespace LIEF::DEX {
	class Field;
}  // namespace LIEF::DEX

namespace sandvik {
	class Class;
	/** @brief Represents a field in a Java class. */
	class Field {
		public:
			/** @brief Constructs a new Field.
			 * @param class_ Reference to the Class that owns this field
			 * @param name_ Name of the field
			 * @param type_ Type of the field
			 * @param isStatic_ Indicates if the field is static
			 */
			Field(Class& class_, const std::string& name_, const std::string& type_, bool isStatic_);
			/** @brief Constructs a Field from a LIEF DEX Field.
			 * @param class_ Reference to the Class that owns this field
			 * @param field_ Reference to the LIEF DEX Field
			 */
			Field(Class& class_, const LIEF::DEX::Field& field_);
			~Field() = default;

			/** @brief Returns a string representation of the field.
			 * @return String representation.
			 */
			std::string str() const;

			/** @brief Gets the class name of the field's type.
			 * @return Class name of the field's type.
			 */
			std::string getFieldTypeClassname() const;

			/** @brief Gets the field's integer (32-bit) value.
			 * @return Integer value.
			 */
			uint32_t getIntValue() const;
			/** @brief Gets the field's long (64-bit) value.
			 * @return Long value.
			 */
			uint64_t getLongValue() const;
			/** @brief Gets the field's string value.
			 * @return String value.
			 */
			std::string getStringValue() const;
			/** @brief Gets the field's Object value.
			 * @return Shared pointer to the Object value.
			 */
			ObjectRef getObjectValue() const;

			/** @brief Sets the field's integer (32-bit) value.
			 * @param value Integer value to set.
			 */
			void setIntValue(uint32_t value);
			/** @brief Sets the field's long (64-bit) value.
			 * @param value Long value to set.
			 */
			void setLongValue(uint64_t value);
			/** @brief Sets the field's string value.
			 * @param value String value to set.
			 */
			void setStringValue(const std::string& value);
			/** @brief Sets the field's Object value.
			 * @param value Shared pointer to the Object value.
			 */
			void setObjectValue(ObjectRef value);

			/** @brief Gets the Class that owns this field.
			 * @return Reference to the Class.
			 */
			Class& getClass() const;
			/** @brief Gets the name of the field.
			 * @return Name of the field.
			 */
			std::string getName() const;
			/** @brief Gets the type of the field.
			 * @return Type of the field.
			 */
			std::string getType() const;
			/** @brief Checks if the field is static.
			 * @return true if the field is static, false otherwise.
			 */
			bool isStatic() const;

			/** Visit outgoing references
			 * @param visitor_ function to call for each referenced object
			 */
			void visitReferences(const std::function<void(Object*)>& visitor_) const;

		private:
			Class& _class;
			std::string _name;
			std::string _type;
			bool _isStatic;

			uint64_t _value;
			std::string _strValue;
			ObjectRef _obj;
	};
}  // namespace sandvik

#endif  // __FIELD_HPP__