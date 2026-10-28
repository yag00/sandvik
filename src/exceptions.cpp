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

#include <cstring>

#include "object.hpp"

using namespace sandvik;

VmException::VmException(const std::string& message_) : _message(message_) {
}

const char* VmException::what() const noexcept {
	return _message.c_str();
}

JavaException::JavaException(const std::string& type_, const std::string& message_) : _type(type_), _message(message_) {
}

const char* JavaException::what() const noexcept {
	return _message.c_str();
}
std::string JavaException::getExceptionType() const {
	return _type;
}
std::string JavaException::getMessage() const {
	return _message;
}

void JavaException::setFields(Object& exception_, ClassLoader& classloader_) const {
	(void)exception_;
	(void)classloader_;
}

ErrnoException::ErrnoException(const std::string& functionName_, int errno_, const std::string& detail_)
    : JavaException("android.system.ErrnoException", detail_.empty() ? fmt::format("{} failed: {}", functionName_, strerror(errno_))
                                                                     : fmt::format("{} failed: {} ({})", functionName_, strerror(errno_), detail_)),
      _functionName(functionName_),
      _errno(errno_) {
}

void ErrnoException::setFields(Object& exception_, ClassLoader& classloader_) const {
	exception_.setField("functionName", Object::make(classloader_, _functionName));
	exception_.setField("errno", Object::make(static_cast<uint64_t>(static_cast<uint32_t>(_errno))));
}
