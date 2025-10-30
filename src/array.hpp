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

#ifndef __ARRAY_HPP__
#define __ARRAY_HPP__

#include <memory>
#include <string>
#include <vector>

#include "object.hpp"

namespace sandvik {
	/** @brief Vector of Object references */
	using ObjectRefVector = std::vector<ObjectRef>;
	/** @brief Array reference type */
	using ArrayRef = std::shared_ptr<Array>;
	/**
	 * @class Array
	 * @brief Base class representing a generic java array.
	 *
	 * The Array class provides an interface for array objects that can store elements, support type checking,
	 * and represent various array types such as one-dimensional and multi-dimensional arrays. Copy and move operations
	 * are explicitly disabled to ensure unique object identity.
	 */
	class Array : public Object {
		public:
			/** @brief Creates a new array object.
			 * @param classtype_ Reference to the Class type of the array
			 * @param size_ Size of the array
			 * @return ObjectRef to the created array object
			 */
			static ObjectRef make(const Class& classtype_, uint32_t size_);
			/** @brief Creates a new multi-dimensional array object.
			 * @param classtype_ Reference to the Class type of the array
			 * @param dimensions_ Vector of dimensions for the array
			 * @return ObjectRef to the created multi-dimensional array object
			 */
			static ObjectRef make(const Class& classtype_, const std::vector<uint32_t>& dimensions_);
			/** @brief Constructs a new Array object.
			 * @param classtype_ Reference to the Class type of the array
			 * @param dimensions_ Vector of dimensions for the array
			 */
			explicit Array(const Class& classtype_, const std::vector<uint32_t>& dimensions_);

			/** @brief Constructor for subarray view
			 * @param data_ Shared pointer to the data vector
			 * @param classtype_ Reference to the Class type of the array
			 * @param dimensions_ Vector of dimensions for the array
			 * @param offset_ Offset for the subarray
			 */
			Array(std::shared_ptr<ObjectRefVector> data_, const Class& classtype_, const std::vector<uint32_t>& dimensions_, size_t offset_);
			~Array() override = default;

			/** @brief Returns a string representation of the array. */
			std::string debug() const override;

			/** @brief Gets the class type of the array.
			 * @return Reference to the Class type of the array.
			 */
			const Class& getClassType() const override;
			/** @brief Checks if the object is an array.
			 * @return true if the object is an array, false otherwise.
			 */
			bool isArray() const override;
			/** @brief Gets the array at the specified index.
			 * @param idx_ Index of the array.
			 * @return Shared pointer to the Array at the specified index.
			 */
			ArrayRef getArray(uint32_t idx_) const;
			/**
			 * @brief An array is considered a class object. All methods of java.lang.Object are available.
			 * @return True
			 */
			bool isClass() const override;
			/**
			 * @brief Gets the Class of the object.
			 * @return Reference a java.lang.Class object.
			 */
			Class& getClass() const override;

			/** @brief Gets the number of dimensions of the array.
			 * @return Number of dimensions.
			 */
			size_t getDimensions() const;
			/** @brief Gets the size of the specified dimension.
			 * @param index_ Index of the dimension.
			 * @return Size of the specified dimension.
			 */
			uint32_t getDimension(uint32_t index_) const;
			/** @brief Gets the size of the first dimension (equivalent to getDimension(0)).
			 * @return Size of the first dimension.
			 */
			uint32_t getArrayLength() const override;

			/** @brief Sets the element at the specified index.
			 * @param idx_ Index of the element.
			 * @param value_ ObjectRef to set at the specified index.
			 */
			void setElement(uint32_t idx_, ObjectRef value_);
			/** @brief Gets the element at the specified index.
			 * @param idx_ Index of the element.
			 * @return ObjectRef at the specified index.
			 */
			ObjectRef getElement(uint32_t idx_) const;

			/** @brief Sets the element at the specified multi-dimensional indices.
			 * @param indices_ Vector of indices for each dimension.
			 * @param value_ ObjectRef to set at the specified indices.
			 */
			void setElement(const std::vector<uint32_t>& indices_, ObjectRef value_);
			/** @brief Gets the element at the specified multi-dimensional indices.
			 * @param indices_ Vector of indices for each dimension.
			 * @return ObjectRef at the specified indices.
			 */
			ObjectRef getElement(const std::vector<uint32_t>& indices_) const;

			/** @brief Clones the array.
			 * @return Shared pointer to the cloned Array.
			 */
			ObjectRef clone() const override;

		private:
			uint32_t flattenIndex(const std::vector<uint32_t>& indices_) const;

			const Class& _classtype;
			std::vector<uint32_t> _dimensions;
			std::shared_ptr<ObjectRefVector> _data;

			size_t _offset;
			size_t _length;
	};
}  // namespace sandvik

#endif  // __ARRAY_HPP__