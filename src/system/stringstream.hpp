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

#ifndef __SYSTEM_STRING_STREAM_HPP__
#define __SYSTEM_STRING_STREAM_HPP__

#include <stdint.h>

#include <system/stream.hpp>
#include <vector>

namespace sandvik {
	/** @class IStringStream
	 *  @brief Input string stream class
	 * performing the write operation on string stream */
	class IStringStream : public Stream {
		public:
			/** Constructor from string data
			 * @param data_ string data */
			explicit IStringStream(const std::string& data_);
			/** Constructor from byte data
			 * @param data_ byte data */
			explicit IStringStream(const std::vector<uint8_t>& data_);
			virtual ~IStringStream();

			/** write method
			 * @param buf_ Buffer to write data from
			 * @param count_ Number of bytes to write
			 * @return number of byte written (should be equal to count)
			 * @throw std::exception */
			virtual long write(const char* buf_, long count_) override;
			/** close the stream */
			virtual void close() override;
	};

	/** @class OStringStream
	 *  @brief Output string stream class
	 * performing the read operation on string stream */
	class OStringStream : public Stream {
		public:
			explicit OStringStream();
			virtual ~OStringStream();

			/** read method
			 * @param buf_ Buffer to read data into
			 * @param count_ Number of bytes to read
			 * @return number of bytes read (should be equal to count)
			 * @throw std::exception */
			virtual long read(char* buf_, long count_) override;
			/** close the stream */
			virtual void close() override;

			/** @return buffer */
			std::string str() const;
	};
}  // namespace sandvik

#endif  // __SYSTEM_STRING_STREAM_HPP__