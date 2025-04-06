#include "zip.hpp"

#include <string.h>

#include <fmt/format.h>

#include <filesystem>
#include <fstream>
#include <stdexcept>

#include "miniz.h"

using namespace sandvik;

ZipReader::ZipReader() {
	_zip = new mz_zip_archive;
	memset(_zip, 0, sizeof(mz_zip_archive));
}

ZipReader::~ZipReader() {
	delete static_cast<mz_zip_archive*>(_zip);
}

void ZipReader::open(const std::string& zipfile_) {
	if (!mz_zip_reader_init_file(static_cast<mz_zip_archive*>(_zip), zipfile_.c_str(), 0)) throw std::runtime_error("zip initialization failed!");
}

void ZipReader::close() {
	if (!mz_zip_reader_end(static_cast<mz_zip_archive*>(_zip))) throw std::runtime_error("zip end failed!");
}

bool ZipReader::isValidArchive(const std::string& zipfile_) {
	std::ifstream ifs(zipfile_.c_str(), std::ifstream::binary);
	if (ifs.is_open()) {
		uint32_t magic;
		ifs.read((char*)&magic, 4);
		ifs.close();
		if (magic == 0x04034b50) return true;
	}
	return false;
}

uint64_t ZipReader::getNumberOfFiles() {
	return static_cast<uint64_t>(mz_zip_reader_get_num_files(static_cast<mz_zip_archive*>(_zip)));
}

void ZipReader::extract(const std::string& file_, const std::string& path_) {
	namespace fs = std::filesystem;

	fs::path root(path_);
	if (!fs::exists(root)) {
		if (!fs::create_directories(root)) {
			throw std::runtime_error(fmt::format("zip can't create directory {}!", path_));
		}
	}

	for (uint64_t i = 0; i < getNumberOfFiles(); i++) {
		mz_zip_archive_file_stat info;
		if (!mz_zip_reader_file_stat(static_cast<mz_zip_archive*>(_zip), i, &info)) {
			throw std::runtime_error("zip failed to retrieve file info!");
		}

		std::string filename(info.m_filename);
		if (file_ == filename) {
			fs::path filePath = root / filename;

			// Ensure the parent directory exists
			if (!fs::exists(filePath.parent_path())) {
				if (!fs::create_directories(filePath.parent_path())) {
					throw std::runtime_error(fmt::format("zip can't create directory {}!", filePath.parent_path().string()));
				}
			}

			// Extract file
			if (!mz_zip_reader_extract_to_file(static_cast<mz_zip_archive*>(_zip), i, filePath.string().c_str(), 0)) {
				throw std::runtime_error(fmt::format("zip can't extract file {} to {}!", filename, filePath.string()));
			}
			break;
		}
	}
}

char* ZipReader::extractToMemory(const std::string& file_, uint64_t& size_) {
	size_t pSize = 0;
	void* ptr = mz_zip_reader_extract_file_to_heap(static_cast<mz_zip_archive*>(_zip), file_.c_str(), &pSize, 0);
	size_ = pSize;
	return static_cast<char*>(ptr);
}

void ZipReader::extractAll(const std::string& path_) {
	namespace fs = std::filesystem;

	fs::path root(path_);
	if (!fs::exists(root)) {
		if (!fs::create_directories(root)) {
			throw std::runtime_error(fmt::format("zip can't create directory {}!", path_));
		}
	}

	for (uint64_t i = 0; i < getNumberOfFiles(); i++) {
		mz_zip_archive_file_stat info;
		if (!mz_zip_reader_file_stat(static_cast<mz_zip_archive*>(_zip), i, &info)) {
			throw std::runtime_error("zip failed to retrieve file info!");
		}

		std::string filename(info.m_filename);
		fs::path filePath = root / filename;

		if (mz_zip_reader_is_file_a_directory(static_cast<mz_zip_archive*>(_zip), i)) {
			// Create directory
			if (!fs::exists(filePath)) {
				if (!fs::create_directories(filePath)) {
					throw std::runtime_error(fmt::format("zip can't create directory {}!", filePath.string()));
				}
			}
		} else {
			// Ensure the parent directory exists
			if (!fs::exists(filePath.parent_path())) {
				if (!fs::create_directories(filePath.parent_path())) {
					throw std::runtime_error(fmt::format("zip can't create directory {}!", filePath.parent_path().string()));
				}
			}

			// Extract file
			if (!mz_zip_reader_extract_to_file(static_cast<mz_zip_archive*>(_zip), i, filePath.string().c_str(), 0)) {
				throw std::runtime_error(fmt::format("zip can't extract file {} to {}!", filename, filePath.string()));
			}
		}
	}
}

std::list<std::string> ZipReader::getList(const std::string& prefix_) {
	std::list<std::string> list;
	for (uint64_t i = 0; i < getNumberOfFiles(); i++) {
		mz_zip_archive_file_stat info;
		if (!mz_zip_reader_file_stat(static_cast<mz_zip_archive*>(_zip), i, &info)) throw std::runtime_error("zip failed to retreive file info!");

		if (!mz_zip_reader_is_file_a_directory(static_cast<mz_zip_archive*>(_zip), i)) {
			list.push_back(prefix_ + std::string(info.m_filename));
		}
	}
	return list;
}

//////////////////////////////////////////////////////////////////////////////////////////
ZipWriter::ZipWriter() {
	_zip = new mz_zip_archive;
	memset(_zip, 0, sizeof(mz_zip_archive));
}

ZipWriter::~ZipWriter() {
	delete static_cast<mz_zip_archive*>(_zip);
}

void ZipWriter::open(const std::string& zipfile_) {
	if (!mz_zip_writer_init_file(static_cast<mz_zip_archive*>(_zip), zipfile_.c_str(), 0)) throw std::runtime_error("zip initialization failed!");
}

void ZipWriter::add(const std::string& archivename_, const std::string& filename_) {
	if (!mz_zip_writer_add_file(static_cast<mz_zip_archive*>(_zip), archivename_.c_str(), filename_.c_str(), 0, 0, MZ_BEST_COMPRESSION))
		throw std::runtime_error(fmt::format("zip can't add {}!", filename_));
}

void ZipWriter::addFromMemory(const std::string& archivename_, const char* data_, uint64_t size_) {
	if (!mz_zip_writer_add_mem(static_cast<mz_zip_archive*>(_zip), archivename_.c_str(), data_, size_, MZ_BEST_COMPRESSION))
		throw std::runtime_error(fmt::format("zip can't add {}!", archivename_));
}

void ZipWriter::close() {
	if (!mz_zip_writer_finalize_archive(static_cast<mz_zip_archive*>(_zip))) throw std::runtime_error("zip finalization failed!");
	if (!mz_zip_writer_end(static_cast<mz_zip_archive*>(_zip))) throw std::runtime_error("zip end failed!");
}
