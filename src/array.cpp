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
