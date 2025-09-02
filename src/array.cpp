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

#include "array.hpp"

#include <fmt/format.h>

#include <algorithm>

#include "class.hpp"
#include "object.hpp"

using namespace sandvik;

std::shared_ptr<Object> Array::make(const Class& classtype_, uint32_t size_) {
	return std::make_shared<Array>(classtype_, std::vector<uint32_t>{size_});
}
std::shared_ptr<Object> Array::make(const Class& classtype_, const std::vector<uint32_t>& dimensions_) {
	return std::make_shared<Array>(classtype_, dimensions_);
}

Array::Array(const Class& classtype_, const std::vector<uint32_t>& dimensions_) : Object(), _classtype(classtype_), _dimensions(dimensions_) {
	uint32_t totalSize = 1;
	for (auto d : _dimensions) {
		totalSize *= d;
	}
	_data.resize(totalSize);
	std::generate(_data.begin(), _data.end(), []() { return Object::makeNull(); });
}

Array::Array(const Array& other) : Object(other), _classtype(other._classtype), _dimensions(other._dimensions), _data(other._data) {
}

std::shared_ptr<Object> Array::clone() const {
	return std::make_shared<Array>(*this);
}

const Class& Array::getClassType() const {
	return _classtype;
}

std::string Array::debug() const {
	std::string dims;
	for (size_t i = 0; i < _dimensions.size(); ++i) {
		dims += std::to_string(_dimensions[i]);
		if (i + 1 < _dimensions.size()) dims += "x";
	}
	return fmt::format("Array type={}, dimensions={}", _classtype.getFullname(), dims);
}

uint32_t Array::getDimensions() const {
	return _dimensions.size();
}

uint32_t Array::getDimension(uint32_t index_) const {
	if (index_ >= _dimensions.size()) {
		throw std::out_of_range("Array dimension index out of bounds");
	}
	return _dimensions[index_];
}

uint32_t Array::getArrayLength() const {
	return getDimension(0);
}

void Array::setElement(uint32_t idx_, std::shared_ptr<Object> value_) {
	if (_dimensions.size() != 1) {
		throw std::invalid_argument("Use multi-dimensional setElement for arrays with more than one dimension");
	}
	if (idx_ >= _data.size()) {
		throw std::out_of_range("Array index out of bounds");
	}
	_data[idx_] = value_;
}

std::shared_ptr<Object> Array::getElement(uint32_t idx_) const {
	if (_dimensions.size() != 1) {
		throw std::invalid_argument("Use multi-dimensional getElement for arrays with more than one dimension");
	}
	if (idx_ >= _data.size()) {
		throw std::out_of_range("Array index out of bounds");
	}
	return _data[idx_];
}

void Array::setElement(const std::vector<uint32_t>& indices_, std::shared_ptr<Object> value_) {
	uint32_t idx = flattenIndex(indices_);
	if (idx >= _data.size()) {
		throw std::out_of_range("Array index out of bounds");
	}
	_data[idx] = value_;
}

std::shared_ptr<Object> Array::getElement(const std::vector<uint32_t>& indices_) const {
	uint32_t idx = flattenIndex(indices_);
	if (idx >= _data.size()) {
		throw std::out_of_range("Array index out of bounds");
	}
	return _data[idx];
}

uint32_t Array::flattenIndex(const std::vector<uint32_t>& indices_) const {
	if (indices_.size() != _dimensions.size()) {
		throw std::invalid_argument("Incorrect number of indices for Array");
	}
	uint32_t idx = 0;
	uint32_t stride = 1;
	for (int i = _dimensions.size() - 1; i >= 0; --i) {
		if (indices_[i] >= _dimensions[i]) {
			throw std::out_of_range("Array index out of bounds");
		}
		idx += indices_[i] * stride;
		stride *= _dimensions[i];
	}
	return idx;
}
