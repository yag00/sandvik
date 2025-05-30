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

#include "filestream.hpp"

#include <fstream>
#include <memory>
#include <stdexcept>

using namespace sandvik;

IFileStream::IFileStream(const std::string& filename) {
	_ios.reset(new std::fstream(filename.c_str(), std::fstream::binary | std::fstream::in));
	std::fstream* ifs = dynamic_cast<std::fstream*>(_ios.get());
	if (!ifs || !ifs->is_open()) {
		throw std::runtime_error("Failed to open file for reading");
	}
	_readStream = true;
}

IFileStream::~IFileStream() {
	close();
}

long IFileStream::write(const char* buf_, long count_) {
	throw std::runtime_error("Write operation not supported on input file stream");
}

void IFileStream::close() {
	std::fstream* ifs = dynamic_cast<std::fstream*>(_ios.get());
	if (ifs && ifs->is_open()) {
		ifs->close();
	}
	_ios.reset();  // Ensure stream is properly released
}

///////////////////////////////////////////////////////////////////////////////

OFileStream::OFileStream(const std::string& filename) {
	_ios.reset(new std::fstream(filename.c_str(), std::fstream::binary | std::fstream::out));
	std::fstream* ofs = dynamic_cast<std::fstream*>(_ios.get());
	if (!ofs || !ofs->is_open()) {
		throw std::runtime_error("Failed to open file for writing");
	}
}

OFileStream::~OFileStream() {
	close();
}

long OFileStream::read(char* buf_, long count_) {
	throw std::runtime_error("Read operation not supported on output file stream");
}

void OFileStream::close() {
	std::fstream* ofs = dynamic_cast<std::fstream*>(_ios.get());
	if (ofs && ofs->is_open()) {
		ofs->flush();
		ofs->close();
	}
	_ios.reset();  // Ensure stream is properly released
}