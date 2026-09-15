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

#ifndef __SYSTEM_COMPRESSION_ZIP_HPP__
#define __SYSTEM_COMPRESSION_ZIP_HPP__

#include <stdint.h>

#include <filesystem>
#include <list>
#include <memory>
#include <string>

namespace sandvik {
	namespace internal {
		/** @brief Opaque handle wrapping the underlying miniz mz_zip_archive, so
		 * zip.hpp doesn't need to include miniz.h; fully defined in zip.cpp. */
		struct ZipArchiveHandle;
	}  // namespace internal

	/** @brief Class for reading zip archives */
	class ZipReader {
		public:
			ZipReader();
			~ZipReader();

			/** @return true if the open file is a valid zip archive
			 * (ie check the magic number of the zip file */
			static bool isValidArchive(const std::string& zipfile_);
			/** open an existing zip archive
			 * @param zipfile_ zip archive name
			 * @throw std::exception */
			void open(const std::string& zipfile_);
			/** open an existing zip archive from memory
			 * @param data_ zip archive data
			 * @param size_ size of data
			 * @throw std::exception */
			void open(const uint8_t* data_, size_t size_);
			/** finalize and close the zip file
			 * @throw std::exception */
			void close();
			/** extract file into the given path
			 * @param file_ file to extract
			 * @param path_ path for zip extraction
			 * @throw std::exception */
			void extract(const std::string& file_, const std::string& path_);
			/** extract file into memory
			 * @param file_ file to extract
			 * @param size_ size of extracted data
			 * @return extracted data as std::unique_ptr<char[]>
			 * @throw std::exception */
			std::unique_ptr<char[]> extractToMemory(const std::string& file_, uint64_t& size_);
			/** extract all into the given path
			 * @param path_ path for zip extraction
			 * @throw std::exception */
			void extractAll(const std::string& path_);
			/** @return list of files in the zip file
			 * @param prefix_ add prefix to each file
			 * @throw std::exception */
			std::list<std::string> getList(const std::string& prefix_ = "");

		protected:
			/** @return the number of entry in the zip file */
			uint64_t getNumberOfFiles();

		private:
			ZipReader(ZipReader& that);
			void operator=(ZipReader& that);

			/** Resolves a zip entry's name against the extraction root, rejecting any
			 * entry whose (possibly ../-laden) name would escape that root.
			 * @throw std::exception if the entry escapes the extraction root */
			static std::filesystem::path resolveEntryPath(const std::filesystem::path& root, const std::string& entryName);

			std::unique_ptr<internal::ZipArchiveHandle> _zip;
	};

	/** @brief Class for writing zip archives */
	class ZipWriter {
		public:
			ZipWriter();
			~ZipWriter();

			/** open a new zip archive
			 * @param zipfile_ zip archive name
			 * @throw std::exception */
			void open(const std::string& zipfile_);
			/** add file to zip archive
			 * @param archivename_ filename in the zip archive
			 * @param filename_ real filename
			 * @throw std::exception */
			void add(const std::string& archivename_, const std::string& filename_);
			/** add file to zip archive
			 * @param archivename_ filename in the zip archive
			 * @param data_ data buffer
			 * @param size_ size of buffer
			 * @throw std::exception */
			void addFromMemory(const std::string& archivename_, const char* data_, uint64_t size_);
			/** finalize and close the zip file
			 * @throw std::exception */
			void close();

		private:
			ZipWriter(ZipWriter& that);
			void operator=(ZipWriter& that);

			std::unique_ptr<internal::ZipArchiveHandle> _zip;
	};
};  // namespace sandvik

#endif  // __SYSTEM_COMPRESSION_ZIP_HPP__
