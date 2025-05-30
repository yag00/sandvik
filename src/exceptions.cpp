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