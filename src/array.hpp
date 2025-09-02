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
			static std::shared_ptr<Object> make(const std::string& type_, uint32_t size_);
			explicit Array(const std::string& type_, uint32_t size_);
			Array(const Array& other);
			~Array() = default;
			std::shared_ptr<Object> clone() const override;
			std::string debug() const override;
			uint32_t getArrayLength() const override;

			void setArrayElement(uint32_t index_, std::shared_ptr<Object> value_);
			std::shared_ptr<Object> getArrayElement(uint32_t index_) const;

		private:
			std::string _type;
			uint32_t _size;
			std::vector<std::shared_ptr<Object>> _data;
	};
	class MultiArray : public Object {
		public:
			static std::shared_ptr<Object> make(const Class& classtype_, const std::vector<uint32_t>& dimensions_);
			explicit MultiArray(const Class& classtype_, const std::vector<uint32_t>& dimensions_);
			MultiArray(const MultiArray& other);
			~MultiArray() = default;
			std::shared_ptr<Object> clone() const override;
			std::string debug() const override;
			uint32_t getArrayLength() const override;

			void setElement(const std::vector<uint32_t>& indices_, std::shared_ptr<Object> value_);
			std::shared_ptr<Object> getElement(const std::vector<uint32_t>& indices_) const;

		private:
			const Class& _classtype;
			std::vector<uint32_t> _dimensions;
			std::vector<std::shared_ptr<Object>> _data;

			uint32_t flattenIndex(const std::vector<uint32_t>& indices_) const;
	};
}  // namespace sandvik

#endif  // __ARRAY_HPP__