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

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include "object.hpp"

namespace sandvik {
	/** @brief A single array element slot.
	 *
	 * All elements of a given Array share the same nature - primitive or object reference -
	 * determined once at construction from the array's component type, so (unlike Frame's
	 * per-register slots) no per-element tag is needed: the owning Array already knows which
	 * union member is active for every element it holds. A primitive array (int[], byte[], ...)
	 * stores raw values inline, unboxed; an object array (Object[], String[], ...) stores an
	 * ObjectRef as before.
	 */
	union ArraySlot {
			uint64_t raw;   ///< Active member for a primitive array element.
			ObjectRef obj;  ///< Active member for an object array element.
	};
	/** @brief Vector of array element slots */
	using ArraySlotVector = std::atomic<ArraySlot>[];
	/** @brief Array reference type */
	using ArrayRef = Array*;
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
			 * @return ArrayRef to the created array object
			 */
			static ArrayRef make(const Class& classtype_, uint32_t size_);
			/** @brief Creates a new multi-dimensional array object.
			 * @param classtype_ Reference to the Class type of the array
			 * @param dimensions_ Vector of dimensions for the array
			 * @return ArrayRef to the created multi-dimensional array object
			 */
			static ArrayRef make(const Class& classtype_, const std::vector<uint32_t>& dimensions_);
			/** @brief Constructs a new Array object.
			 * @param classtype_ Reference to the Class type of the array
			 * @param dimensions_ Vector of dimensions for the array
			 */
			explicit Array(const Class& classtype_, const std::vector<uint32_t>& dimensions_);

			/** @brief Constructor for subarray view
			 * @param data_ Shared pointer to the data vector
			 * @param size_ Size of the data vector
			 * @param classtype_ Reference to the Class type of the array
			 * @param dimensions_ Vector of dimensions for the array
			 * @param offset_ Offset for the subarray
			 */
			Array(std::shared_ptr<ArraySlotVector> data_, uint32_t size_, const Class& classtype_, const std::vector<uint32_t>& dimensions_, size_t offset_);
			~Array() override = default;

			/** @brief Returns a string representation of the array. */
			std::string toString() const override;

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

			/** @brief Checks whether this is a primitive-typed array (int[], byte[], ...) as opposed
			 * to an array of object references (Object[], String[], ...).
			 * @return true if this array stores raw primitive values.
			 */
			bool isPrimitiveArray() const;

			/** @brief Sets the element at the specified index as a raw 64-bit primitive value, without
			 * boxing. Only valid for a primitive array; throws otherwise.
			 * @param idx_ Index of the element.
			 * @param value_ Raw value to set (interpreted by the caller based on the opcode's type:
			 *   the low 8/16/32 bits for byte/char-short/int, or the full 64 bits for long/double).
			 */
			void setRawElement(uint32_t idx_, uint64_t value_);
			/** @brief Gets the element at the specified index as a raw 64-bit primitive value, without
			 * unboxing. Valid for a primitive array; for an object array, falls back to unboxing a
			 * NumberObject element if present (defensive handling of a type-mismatched access), and
			 * throws otherwise.
			 * @param idx_ Index of the element.
			 * @return Raw value of the element.
			 */
			uint64_t getRawElement(uint32_t idx_) const;

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

			/** @brief Atomically compares and sets the element at the specified index.
			 * @param idx_ Index of the element.
			 * @param expected_ Expected ObjectRef value.
			 * @param newValue_ New ObjectRef value to set if the current value matches the expected value.
			 * @return true if the value was set, false otherwise.
			 */
			bool compareAndSwapElement(uint32_t idx_, ObjectRef expected_, ObjectRef newValue_);

			/** @brief Clones the array.
			 * @return Shared pointer to the cloned Array.
			 */
			ObjectRef clone() const override;

			/** Visit outgoing references
			 * @param visitor_ function to call for each referenced object
			 */
			void visitReferences(const std::function<void(Object*)>& visitor_) const override;

		private:
			uint32_t flattenIndex(const std::vector<uint32_t>& indices_) const;

			const Class& _classtype;
			std::vector<uint32_t> _dimensions;
			std::shared_ptr<ArraySlotVector> _data;
			bool _isPrimitive;

			size_t _offset;
			size_t _length;
	};
}  // namespace sandvik

#endif  // __ARRAY_HPP__