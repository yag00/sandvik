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

			/** read method
			 * @return number of bytes read (should be equal to count)
			 * @throw std::exception */
			virtual long read(char* buf_, long count_);
			/** write method
			 * @return number of byte written (should be equal to count)
			 * @throw std::exception */
			virtual long write(const char* buf_, long count_);
			/** flush the stream */
			virtual void flush();
			/** seek method relative to the start of the stream
			 * @throw std::exception */
			virtual void seek(long offset);
			/** tell method gives the current position in the stream
			 * @throw std::exception */
			virtual long tell();
			/** close the stream */
			virtual void close();

			uint8_t readU1();
			uint16_t readU2();
			uint32_t readU4();

		protected:
			/** iostream */
			std::unique_ptr<std::iostream> _ios;
			bool _readStream;

		private:
			Stream(Stream& that);
			void operator=(Stream& that);
	};
};  // namespace sandvik

#endif /* __SYSTEM_STREAM_HPP__ */
