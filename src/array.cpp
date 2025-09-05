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

ObjectRef Array::make(const Class& classtype_, uint32_t size_) {
	return std::make_shared<Array>(classtype_, std::vector<uint32_t>{size_});
}
ObjectRef Array::make(const Class& classtype_, const std::vector<uint32_t>& dimensions_) {
	return std::make_shared<Array>(classtype_, dimensions_);
}

Array::Array(const Class& classtype_, const std::vector<uint32_t>& dimensions_)
    : Object(), _classtype(classtype_), _dimensions(dimensions_), _offset(0), _length(0) {
	uint32_t totalSize = 1;
	for (auto d : _dimensions) {
		totalSize *= d;
	}
	_length = totalSize;
	_data = std::make_shared<ObjectRefVector>();
	_data->resize(totalSize);
	std::generate(_data->begin(), _data->end(), []() { return Object::makeNull(); });
}

Array::Array(std::shared_ptr<ObjectRefVector> data_, const Class& classtype_, const std::vector<uint32_t>& dimensions_, size_t offset_)
    : Object(), _classtype(classtype_), _dimensions(dimensions_), _data(data_), _offset(offset_) {
	uint32_t totalSize = 1;
	for (auto d : _dimensions) {
		totalSize *= d;
	}
	_length = totalSize;
	if (_offset + _length > _data->size()) {
		throw std::out_of_range("Subarray out of range");
	}
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

size_t Array::getDimensions() const {
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

void Array::setElement(uint32_t idx_, ObjectRef value_) {
	if (_dimensions.size() != 1) {
		throw std::invalid_argument("Use multi-dimensional setElement for arrays with more than one dimension");
	}
	if (idx_ >= getArrayLength()) {
		throw std::out_of_range("Array index out of bounds");
	}
	(*_data)[_offset + idx_] = value_;
}

ObjectRef Array::getElement(uint32_t idx_) const {
	if (_dimensions.size() != 1) {
		return getArray(idx_);
	}
	if (idx_ >= getArrayLength()) {
		throw std::out_of_range("Array index out of bounds");
	}
	return (*_data)[_offset + idx_];
}

void Array::setElement(const std::vector<uint32_t>& indices_, ObjectRef value_) {
	uint32_t idx = flattenIndex(indices_);
	if (idx >= _data->size()) {
		throw std::out_of_range("Array index out of bounds");
	}
	(*_data)[idx] = value_;
}

ObjectRef Array::getElement(const std::vector<uint32_t>& indices_) const {
	uint32_t idx = flattenIndex(indices_);
	if (idx >= _data->size()) {
		throw std::out_of_range("Array index out of bounds");
	}
	return (*_data)[idx];
}

uint32_t Array::flattenIndex(const std::vector<uint32_t>& indices_) const {
	if (indices_.size() != _dimensions.size()) {
		throw std::invalid_argument("Incorrect number of indices for Array");
	}
	uint32_t idx = 0;
	for (size_t i = 0; i < _dimensions.size(); ++i) {
		if (indices_[i] >= _dimensions[i]) {
			throw std::out_of_range("Array index out of bounds");
		}
		idx = idx * _dimensions[i] + indices_[i];
	}
	return idx;
}

std::shared_ptr<Array> Array::getArray(uint32_t idx_) const {
	// Ensure the array is multi-dimensional
	if (_dimensions.size() <= 1) {
		throw std::invalid_argument("Cannot get a sub-array from a one-dimensional array");
	}
	// Check if the index is within bounds
	if (idx_ >= _dimensions[0]) {
		throw std::out_of_range("Array index out of bounds");
	}
	// Calculate the size of the sub-array
	uint32_t subArraySize = 1;
	for (size_t i = 1; i < _dimensions.size(); ++i) {
		subArraySize *= _dimensions[i];
	}
	// Calculate the starting index of the sub-array in the flattened data
	uint32_t startIdx = idx_ * subArraySize;
	// Create a sub-array that references the original data
	return std::make_shared<Array>(_data, _classtype, std::vector<uint32_t>(_dimensions.begin() + 1, _dimensions.end()), _offset + startIdx);
}
