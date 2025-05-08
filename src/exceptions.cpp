#include "exceptions.hpp"

#include <fmt/format.h>

using namespace sandvik;

StackOverflowError::StackOverflowError() {
}

StackOverflowError::~StackOverflowError() noexcept {
}

const char* StackOverflowError::what() const noexcept {
	return "StackOverflowError";
}

///////////////////////////////////////////////////////////////////////////////
OutOfMemoryError::OutOfMemoryError() {
}

OutOfMemoryError::~OutOfMemoryError() noexcept {
}

const char* OutOfMemoryError::what() const noexcept {
	return "OutOfMemoryError";
}
///////////////////////////////////////////////////////////////////////////////
NullPointerException::NullPointerException(const std::string& message) {
	_message = fmt::format("NullPointerException: {}", message);
}

NullPointerException::~NullPointerException() noexcept {
}

const char* NullPointerException::what() const noexcept {
	return _message.c_str();
}