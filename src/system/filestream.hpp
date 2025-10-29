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

#ifndef __SYSTEM_FILESTREAM_HPP__
#define __SYSTEM_FILESTREAM_HPP__

#include <system/stream.hpp>

namespace sandvik {
	/** @brief Input file stream */
	class IFileStream : public Stream {
		public:
			/** Constructor from filename
			 * @param filename Filename to open */
			explicit IFileStream(const std::string& filename);
			virtual ~IFileStream();

			/** write method
			 * @param buf_ Buffer to write data from
			 * @param count_ Number of bytes to write
			 * @return number of byte written (should be equal to count)
			 * @throw std::exception */
			virtual long write(const char* buf_, long count_) override;
			/** close the stream */
			virtual void close() override;

		private:
			void __close();
	};
	/** @brief Output file stream */
	class OFileStream : public Stream {
		public:
			/** Constructor from filename
			 * @param filename Filename to open */
			explicit OFileStream(const std::string& filename);
			virtual ~OFileStream();

			/** read method
			 * @param buf_ Buffer to read data into
			 * @param count_ Number of bytes to read
			 * @return number of bytes read (should be equal to count)
			 * @throw std::exception */
			virtual long read(char* buf_, long count_) override;
			/** close the stream */
			virtual void close() override;

		private:
			void __close();
	};
}  // namespace sandvik

#endif  // __SYSTEM_FILESTREAM_HPP__