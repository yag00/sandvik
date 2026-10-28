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

#ifndef __RESOURCES_STRING_POOL_HPP__
#define __RESOURCES_STRING_POOL_HPP__

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "resources/resource_types.hpp"

namespace sandvik::resources {
	/**
	 * @class StringPool
	 * @brief Read-only view of a ResStringPool chunk (androidfw ResStringPool).
	 *
	 * The pool does not copy the strings: it keeps the buffer holding the chunk alive and decodes strings on demand.
	 */
	class StringPool {
		public:
			StringPool() = default;
			/** @brief Parses a string pool chunk.
			 * @param chunk_ view starting at the ResStringPool_header, of the chunk size
			 * @param owner_ buffer the view points into, kept alive by the pool */
			StringPool(ByteView chunk_, std::shared_ptr<const void> owner_);

			/** @brief Creates a pool owning a copy of a serialized chunk (StringBlock.nativeCreate) */
			static std::shared_ptr<StringPool> fromBytes(const uint8_t* data_, size_t size_);

			uint32_t size() const {
				return _header.stringCount;
			}
			bool isUtf8() const {
				return (_header.flags & STRING_POOL_UTF8_FLAG) != 0;
			}
			/** @brief String at index as UTF-16 (Java string), nullopt if out of range */
			std::optional<std::u16string> stringAt(uint32_t index_) const;
			/** @brief String at index as UTF-8, nullopt if out of range */
			std::optional<std::string> string8At(uint32_t index_) const;
			/** @brief Style spans of the string at index (empty if the string has no style) */
			std::vector<ResStringPoolSpan> styleAt(uint32_t index_) const;

		private:
			ByteView _chunk;
			std::shared_ptr<const void> _owner;
			ResStringPoolHeader _header{};
	};

}  // namespace sandvik::resources

#endif  // __RESOURCES_STRING_POOL_HPP__
