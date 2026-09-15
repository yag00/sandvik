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

#include "zip.hpp"

#include <string.h>

#include <fmt/format.h>

#include <filesystem>
#include <fstream>
#include <stdexcept>

#include "miniz.h"

using namespace sandvik;

namespace sandvik::internal {
	struct ZipArchiveHandle : mz_zip_archive {};
}  // namespace sandvik::internal

ZipReader::ZipReader() {
	_zip = std::make_unique<internal::ZipArchiveHandle>();
	memset(_zip.get(), 0, sizeof(mz_zip_archive));
}

ZipReader::~ZipReader() = default;

void ZipReader::open(const uint8_t* data_, size_t size_) {
	if (!mz_zip_reader_init_mem(_zip.get(), data_, size_, 0)) {
		throw std::runtime_error("zip initialization from memory failed!");
	}
}

void ZipReader::open(const std::string& zipfile_) {
	if (!mz_zip_reader_init_file(_zip.get(), zipfile_.c_str(), 0)) throw std::runtime_error("zip initialization failed!");
}

void ZipReader::close() {
	if (!mz_zip_reader_end(_zip.get())) throw std::runtime_error("zip end failed!");
}

bool ZipReader::isValidArchive(const std::string& zipfile_) {
	namespace fs = std::filesystem;

	// Canonicalize before touching the filesystem so that any ../ components or
	// symlinks are resolved, and reject anything that isn't a plain regular file.
	std::error_code ec;
	fs::path resolved = fs::canonical(zipfile_, ec);
	if (ec || !fs::is_regular_file(resolved, ec)) {
		return false;
	}

	std::ifstream ifs(resolved, std::ifstream::binary);
	if (ifs.is_open()) {
		uint32_t magic;
		ifs.read((char*)&magic, 4);
		ifs.close();
		if (magic == 0x04034b50) return true;
	}
	return false;
}

uint64_t ZipReader::getNumberOfFiles() {
	return static_cast<uint64_t>(mz_zip_reader_get_num_files(_zip.get()));
}

std::filesystem::path ZipReader::resolveEntryPath(const std::filesystem::path& root, const std::string& entryName) {
	namespace fs = std::filesystem;

	// Zip entry names come from the (untrusted) archive content: without this check a
	// crafted entry such as "../../etc/passwd" or an absolute path would let extraction
	// write outside of `root` (a.k.a. "Zip Slip", CWE-22).
	fs::path filePath = fs::weakly_canonical(root / entryName);
	fs::path canonicalRoot = fs::weakly_canonical(root);
	const auto relative = filePath.lexically_relative(canonicalRoot);
	if (relative.empty() || relative.string().find("..") != std::string::npos) {
		throw std::runtime_error(fmt::format("zip entry escapes extraction directory: {}", entryName));
	}
	return filePath;
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
		if (!mz_zip_reader_file_stat(_zip.get(), i, &info)) {
			throw std::runtime_error("zip failed to retrieve file info!");
		}

		std::string filename(info.m_filename);
		if (file_ == filename) {
			fs::path filePath = resolveEntryPath(root, filename);

			// Ensure the parent directory exists
			if (!fs::exists(filePath.parent_path())) {
				if (!fs::create_directories(filePath.parent_path())) {
					throw std::runtime_error(fmt::format("zip can't create directory {}!", filePath.parent_path().string()));
				}
			}

			// Extract file
			if (!mz_zip_reader_extract_to_file(_zip.get(), i, filePath.string().c_str(), 0)) {
				throw std::runtime_error(fmt::format("zip can't extract file {} to {}!", filename, filePath.string()));
			}
			break;
		}
	}
}

std::unique_ptr<char[]> ZipReader::extractToMemory(const std::string& file_, uint64_t& size_) {
	size_t pSize = 0;
	void* ptr = mz_zip_reader_extract_file_to_heap(_zip.get(), file_.c_str(), &pSize, 0);
	if (!ptr) {
		throw std::runtime_error(fmt::format("zip can't extract file {} to memory!", file_));
	}
	size_ = pSize;
	std::unique_ptr<char[]> result(new char[size_]);
	memcpy(result.get(), ptr, size_);
	mz_free(ptr);
	return result;
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
		if (!mz_zip_reader_file_stat(_zip.get(), i, &info)) {
			throw std::runtime_error("zip failed to retrieve file info!");
		}

		std::string filename(info.m_filename);
		fs::path filePath = resolveEntryPath(root, filename);

		if (mz_zip_reader_is_file_a_directory(_zip.get(), i)) {
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
			if (!mz_zip_reader_extract_to_file(_zip.get(), i, filePath.string().c_str(), 0)) {
				throw std::runtime_error(fmt::format("zip can't extract file {} to {}!", filename, filePath.string()));
			}
		}
	}
}

std::list<std::string> ZipReader::getList(const std::string& prefix_) {
	std::list<std::string> list;
	for (uint64_t i = 0; i < getNumberOfFiles(); i++) {
		mz_zip_archive_file_stat info;
		if (!mz_zip_reader_file_stat(_zip.get(), i, &info)) throw std::runtime_error("zip failed to retreive file info!");

		if (!mz_zip_reader_is_file_a_directory(_zip.get(), i)) {
			list.push_back(prefix_ + std::string(info.m_filename));
		}
	}
	return list;
}

//////////////////////////////////////////////////////////////////////////////////////////
ZipWriter::ZipWriter() {
	_zip = std::make_unique<internal::ZipArchiveHandle>();
	memset(_zip.get(), 0, sizeof(mz_zip_archive));
}

ZipWriter::~ZipWriter() = default;

void ZipWriter::open(const std::string& zipfile_) {
	if (!mz_zip_writer_init_file(_zip.get(), zipfile_.c_str(), 0)) throw std::runtime_error("zip initialization failed!");
}

void ZipWriter::add(const std::string& archivename_, const std::string& filename_) {
	if (!mz_zip_writer_add_file(_zip.get(), archivename_.c_str(), filename_.c_str(), nullptr, 0, MZ_BEST_COMPRESSION))
		throw std::runtime_error(fmt::format("zip can't add {}!", filename_));
}

void ZipWriter::addFromMemory(const std::string& archivename_, const char* data_, uint64_t size_) {
	if (!mz_zip_writer_add_mem(_zip.get(), archivename_.c_str(), data_, size_, MZ_BEST_COMPRESSION))
		throw std::runtime_error(fmt::format("zip can't add {}!", archivename_));
}

void ZipWriter::close() {
	if (!mz_zip_writer_finalize_archive(_zip.get())) throw std::runtime_error("zip finalization failed!");
	if (!mz_zip_writer_end(_zip.get())) throw std::runtime_error("zip end failed!");
}
