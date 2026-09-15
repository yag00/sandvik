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

#include "stringstream.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace sandvik;

///////////////////////////////////////////////////////////////////////////////
// IStringStream (Input from string)
///////////////////////////////////////////////////////////////////////////////

IStringStream::IStringStream(const std::string& data) {
	setStream(std::make_unique<std::stringstream>(data, std::stringstream::binary | std::stringstream::in));
	if (!stream()) {
		throw std::runtime_error("Failed to initialize input string stream");
	}
	setReadStream(true);
}

IStringStream::IStringStream(const std::vector<uint8_t>& data) {
	// Convert vector<uint8_t> to std::string
	std::string strData(data.begin(), data.end());
	// Initialize as a string stream
	setStream(std::make_unique<std::stringstream>(strData, std::stringstream::binary | std::stringstream::in));
	if (!stream()) {
		throw std::runtime_error("Failed to initialize input string stream from vector");
	}
	setReadStream(true);
}

IStringStream::~IStringStream() = default;

long IStringStream::write(const char* buf_, long count_) {
	throw std::runtime_error("Write operation not supported on input string stream");
}

void IStringStream::close() {
	setStream(nullptr);  // Release the stream
}

///////////////////////////////////////////////////////////////////////////////
// OStringStream (Output to string)
///////////////////////////////////////////////////////////////////////////////

OStringStream::OStringStream() {
	setStream(std::make_unique<std::stringstream>(std::stringstream::binary | std::stringstream::out));
	if (!stream()) {
		throw std::runtime_error("Failed to initialize output string stream");
	}
}

OStringStream::~OStringStream() = default;

long OStringStream::read(char* buf_, long count_) {
	throw std::runtime_error("Read operation not supported on output string stream");
}

void OStringStream::close() {
	setStream(nullptr);  // Release the stream
}

std::string OStringStream::str() const {
	std::stringstream* oss = dynamic_cast<std::stringstream*>(stream());
	if (!oss) {
		throw std::runtime_error("Invalid output string stream");
	}
	return oss->str();
}
