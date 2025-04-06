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