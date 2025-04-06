#include "stringstream.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace sandvik;

///////////////////////////////////////////////////////////////////////////////
// IStringStream (Input from string)
///////////////////////////////////////////////////////////////////////////////

IStringStream::IStringStream(const std::string& data) {
	_ios.reset(new std::stringstream(data, std::stringstream::binary | std::stringstream::in));
	if (!_ios) {
		throw std::runtime_error("Failed to initialize input string stream");
	}
	_readStream = true;
}

IStringStream::IStringStream(const std::vector<uint8_t>& data) {
	// Convert vector<uint8_t> to std::string
	std::string strData(data.begin(), data.end());
	// Initialize as a string stream
	_ios.reset(new std::stringstream(strData, std::stringstream::binary | std::stringstream::in));
	if (!_ios) {
		throw std::runtime_error("Failed to initialize input string stream from vector");
	}
	_readStream = true;
}

IStringStream::~IStringStream() {
	close();
}

long IStringStream::write(const char* buf_, long count_) {
	throw std::runtime_error("Write operation not supported on input string stream");
}

void IStringStream::close() {
	_ios.reset();  // Release the stream
}

///////////////////////////////////////////////////////////////////////////////
// OStringStream (Output to string)
///////////////////////////////////////////////////////////////////////////////

OStringStream::OStringStream() {
	_ios.reset(new std::stringstream(std::stringstream::binary | std::stringstream::out));
	if (!_ios) {
		throw std::runtime_error("Failed to initialize output string stream");
	}
}

OStringStream::~OStringStream() {
	close();
}

long OStringStream::read(char* buf_, long count_) {
	throw std::runtime_error("Read operation not supported on output string stream");
}

void OStringStream::close() {
	_ios.reset();  // Release the stream
}

std::string OStringStream::str() const {
	std::stringstream* oss = dynamic_cast<std::stringstream*>(_ios.get());
	if (!oss) {
		throw std::runtime_error("Invalid output string stream");
	}
	return oss->str();
}
