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

#include "stream.hpp"

#include <byteswap.h>

#include <stdexcept>

using namespace sandvik;

Stream::Stream() : _ios(nullptr), _readStream(false) {
}

Stream::~Stream() {
	if (_ios) {
		_ios->flush();
	}
}

long Stream::read(char* buf_, long count_) {
	if (!_ios || !_ios->good()) {
		throw std::runtime_error("Stream is not in a good state for reading");
	}
	_ios->read(buf_, count_);
	return _ios->gcount();
}

long Stream::write(const char* buf_, long count_) {
	if (!_ios || !_ios->good()) {
		throw std::runtime_error("Stream is not in a good state for writing");
	}
	_ios->write(buf_, count_);
	if (_ios->fail()) {
		throw std::runtime_error("Failed to write to stream");
	}
	return count_;
}

void Stream::flush() {
	if (_ios) {
		_ios->flush();
	}
}

void Stream::seek(long offset) {
	if (_readStream)
		_ios->seekg(offset);
	else
		_ios->seekp(offset);
}

long Stream::tell() {
	if (_readStream)
		return _ios->tellg();
	else
		return _ios->tellp();
}

void Stream::close() {
	flush();
}

uint8_t Stream::readU1() {
	uint8_t value = 0;
	read((char*)&value, 1);
	return value;
}

uint16_t Stream::readU2() {
	uint16_t value = 0;
	read((char*)&value, 2);
	return bswap_16(value);
}

uint32_t Stream::readU4() {
	uint32_t value = 0;
	read((char*)&value, 4);
	return bswap_32(value);
}
