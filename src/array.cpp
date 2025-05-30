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

#include "object.hpp"

using namespace sandvik;

std::shared_ptr<Object> Array::make(const std::string& type_, uint32_t size_) {
	return std::make_shared<Array>(type_, size_);
}
Array::Array(const std::string& type_, uint32_t size_) : Object(), _type(type_), _size(size_) {
	_data.resize(size_);
	std::generate(_data.begin(), _data.end(), []() { return Object::makeNull(); });
}

Array::Array(const Array& other) : Object(other), _type(other._type), _size(other._size), _data(other._data) {
}

std::shared_ptr<Object> Array::clone() const {
	return std::make_shared<Array>(*this);
}

std::string Array::debug() const {
	return fmt::format("Array type={}, size={}", _type, _size);
}

uint32_t Array::getArrayLength() const {
	return _size;
}

void Array::setArrayElement(uint32_t index_, std::shared_ptr<Object> value_) {
	if (index_ >= _size) {
		throw std::out_of_range("Array index out of bounds");
	}
	// todo check type
	// if (value_ == nullptr || value_->getType() != _type) {
	//	throw std::invalid_argument("Invalid object type for array element");
	//}
	_data[index_] = value_;
}
std::shared_ptr<Object> Array::getArrayElement(uint32_t index_) const {
	if (index_ >= _size) {
		throw std::out_of_range("Array index out of bounds");
	}
	return _data[index_];
}
