#include "stream.hpp"

#include <byteswap.h>

#include <stdexcept>

using namespace sandvik;

Stream::Stream() : _ios(nullptr), _readStream(false) {
}

Stream::~Stream() {
	close();
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

Stream::Stream(Stream& that) {
	// Copy constructor is private and not implemented
}

void Stream::operator=(Stream& that) {
	// Assignment operator is private and not implemented
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
