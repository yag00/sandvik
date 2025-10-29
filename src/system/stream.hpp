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

#ifndef __SYSTEM_STREAM_HPP__
#define __SYSTEM_STREAM_HPP__

#include <stdint.h>

#include <iostream>
#include <memory>

namespace sandvik {
	/** @class Stream
	 *  @brief Stream class
	 * performing the read/write operation on stream */
	class Stream {
		public:
			/** Default constructor */
			Stream();
			/** Destructor */
			virtual ~Stream();
			/** Prevent copying */
			Stream(Stream& that) = delete;
			/** Prevent assignment */
			void operator=(Stream& that) = delete;

			/** read method
			 * @param buf_ Buffer to read data into
			 * @param count_ Number of bytes to read
			 * @return number of bytes read (should be equal to count)
			 * @throw std::exception */
			virtual long read(char* buf_, long count_);
			/** write method
			 * @param buf_ Buffer to write data from
			 * @param count_ Number of bytes to write
			 * @return number of byte written (should be equal to count)
			 * @throw std::exception */
			virtual long write(const char* buf_, long count_);
			/** flush the stream */
			virtual void flush();
			/** seek method relative to the start of the stream
			 * @param offset_ Offset to seek to
			 * @throw std::exception */
			virtual void seek(long offset_);
			/** tell method gives the current position in the stream
			 * @throw std::exception */
			virtual long tell();
			/** close the stream */
			virtual void close();

			/** read unsigned 1 byte
			 * @return value read
			 * @throw std::exception */
			uint8_t readU1();
			/** read unsigned 2 bytes big endian
			 * @return value read
			 * @throw std::exception */
			uint16_t readU2();
			/** read unsigned 4 bytes big endian
			 * @return value read
			 * @throw std::exception */
			uint32_t readU4();

		protected:
			/** iostream */
			std::unique_ptr<std::iostream> _ios;
			/** true if read stream, false if write stream */
			bool _readStream;
	};
};  // namespace sandvik

#endif /* __SYSTEM_STREAM_HPP__ */
