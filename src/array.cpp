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
#include "exceptions.hpp"
#include "gc.hpp"
#include "monitor.hpp"
#include "object.hpp"

using namespace sandvik;

ArrayRef Array::make(const Class& classtype_, uint32_t size_) {
	return Array::make(classtype_, std::vector<uint32_t>{size_});
}
ArrayRef Array::make(const Class& classtype_, const std::vector<uint32_t>& dimensions_) {
	auto u = std::make_unique<Array>(classtype_, dimensions_);
	auto ptr = u.get();
	GC::getInstance().track(std::move(u));
	return ptr;
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

bool Array::isArray() const {
	return true;
}

bool Array::isClass() const {
	return true;
}

Class& Array::getClass() const {
	// If the array's class is already java.lang.Object, return it.
	if (_classtype.getFullname() == "java.lang.Object") {
		return const_cast<Class&>(_classtype);
	}
	// Walk the superclass chain (inspect as const) and return the java.lang.Object entry if found.
	const Class* cur = &_classtype;
	while (cur->hasSuperClass()) {
		auto& super = cur->getSuperClass();
		if (super.getFullname() == "java.lang.Object") {
			return super;
		}
		cur = &super;
	}
	throw VmException("Array does not have java.lang.Object as superclass");
}

const Class& Array::getClassType() const {
	return _classtype;
}

std::string Array::toString() const {
	std::string dims;
	for (size_t i = 0; i < _dimensions.size(); ++i) {
		dims += std::to_string(_dimensions[i]);
		if (i + 1 < _dimensions.size()) dims += "x";
	}
	return fmt::format("{}[{}]", _classtype.getFullname(), dims);
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

ArrayRef Array::getArray(uint32_t idx_) const {
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
	auto subArray = std::make_unique<Array>(_data, _classtype, std::vector<uint32_t>(_dimensions.begin() + 1, _dimensions.end()), _offset + startIdx);
	auto ptr = subArray.get();
	GC::getInstance().track(std::move(subArray));
	return ptr;
}

ObjectRef Array::clone() const {
	auto newArray = (ArrayRef)Array::make(_classtype, _dimensions);
	for (uint32_t i = 0; i < _length; ++i) {
		(*newArray->_data)[i] = (*_data)[_offset + i];
	}
	return newArray;
}

void Array::visitReferences(const std::function<void(Object*)>& visitor_) const {
	Object::visitReferences(visitor_);
	for (size_t i = 0; i < _length; ++i) {
		Object* obj = (*_data)[_offset + i];
		if (obj != nullptr) {
			visitor_(obj);
		}
	}
}
