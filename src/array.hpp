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
	class Array : public Object {
		public:
			using ObjectVector = std::vector<std::shared_ptr<Object>>;

			static std::shared_ptr<Object> make(const Class& classtype_, uint32_t size_);
			static std::shared_ptr<Object> make(const Class& classtype_, const std::vector<uint32_t>& dimensions_);
			explicit Array(const Class& classtype_, const std::vector<uint32_t>& dimensions_);
			Array(const Array& other);
			// Constructor for subarray view
			Array(std::shared_ptr<ObjectVector> data_, const Class& classtype_, const std::vector<uint32_t>& dimensions_, size_t offset_);
			~Array() = default;

			std::shared_ptr<Object> clone() const override;
			std::string debug() const override;

			const Class& getClassType() const;

			std::shared_ptr<Array> getArray(uint32_t idx_) const;

			uint32_t getDimensions() const;
			uint32_t getDimension(uint32_t index_) const;
			uint32_t getArrayLength() const override;

			void setElement(uint32_t idx_, std::shared_ptr<Object> value_);
			std::shared_ptr<Object> getElement(uint32_t idx_) const;

			void setElement(const std::vector<uint32_t>& indices_, std::shared_ptr<Object> value_);
			std::shared_ptr<Object> getElement(const std::vector<uint32_t>& indices_) const;

		private:
			uint32_t flattenIndex(const std::vector<uint32_t>& indices_) const;

			const Class& _classtype;
			std::vector<uint32_t> _dimensions;
			std::shared_ptr<ObjectVector> _data;

			size_t _offset;
			size_t _length;
	};
}  // namespace sandvik

#endif  // __ARRAY_HPP__