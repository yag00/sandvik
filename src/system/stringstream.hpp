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
	class IStringStream : public Stream {
		public:
			IStringStream(const std::string& data);
			IStringStream(const std::vector<uint8_t>& data);

			virtual ~IStringStream();

			virtual long write(const char* buf_, long count_) override;
			virtual void close() override;
	};

	class OStringStream : public Stream {
		public:
			OStringStream();
			virtual ~OStringStream();

			virtual long read(char* buf_, long count_) override;
			virtual void close() override;

			/** @return buffer */
			std::string str() const;
	};
}  // namespace sandvik

#endif  // __SYSTEM_STRING_STREAM_HPP__