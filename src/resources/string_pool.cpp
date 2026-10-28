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

#include "resources/string_pool.hpp"

#include "utils.hpp"

using namespace sandvik;
using namespace sandvik::resources;

StringPool::StringPool(ByteView chunk_, std::shared_ptr<const void> owner_) : _chunk(chunk_), _owner(std::move(owner_)) {
	_header = _chunk.read<ResStringPoolHeader>(0);
	if (_header.header.type != RES_STRING_POOL_TYPE) {
		throw VmException("resources: chunk type {:#x} is not a string pool", _header.header.type);
	}
	// validate the offset tables once, string accesses are then bounds checked individually
	_chunk.sub(_header.header.headerSize, (static_cast<size_t>(_header.stringCount) + _header.styleCount) * sizeof(uint32_t));
}

std::shared_ptr<StringPool> StringPool::fromBytes(const uint8_t* data_, size_t size_) {
	auto buffer = std::make_shared<std::vector<uint8_t>>(data_, data_ + size_);
	return std::make_shared<StringPool>(ByteView(buffer->data(), buffer->size()), buffer);
}

std::optional<std::u16string> StringPool::stringAt(uint32_t index_) const {
	if (index_ >= _header.stringCount) {
		return std::nullopt;
	}
	if (isUtf8()) {
		auto str = string8At(index_);
		return str ? std::optional<std::u16string>(utf8_to_utf16(*str)) : std::nullopt;
	}
	const auto offset = _header.stringsStart + _chunk.read<uint32_t>(_header.header.headerSize + index_ * sizeof(uint32_t));
	// length in UTF-16 units, on 2 units when the high bit is set
	size_t pos = offset;
	uint32_t length = _chunk.read<uint16_t>(pos);
	pos += 2;
	if (length & 0x8000) {
		length = ((length & 0x7fff) << 16) | _chunk.read<uint16_t>(pos);
		pos += 2;
	}
	auto chars = _chunk.sub(pos, static_cast<size_t>(length) * sizeof(char16_t));
	std::u16string result(length, u'\0');
	std::memcpy(result.data(), chars.data(), chars.size());
	return result;
}

std::optional<std::string> StringPool::string8At(uint32_t index_) const {
	if (index_ >= _header.stringCount) {
		return std::nullopt;
	}
	if (!isUtf8()) {
		auto str = stringAt(index_);
		return str ? std::optional<std::string>(utf16_to_utf8(*str)) : std::nullopt;
	}
	const auto offset = _header.stringsStart + _chunk.read<uint32_t>(_header.header.headerSize + index_ * sizeof(uint32_t));
	// UTF-16 length then UTF-8 length, each on 1 byte or 2 bytes when the high bit is set
	size_t pos = offset;
	auto readLength = [this, &pos]() {
		uint32_t length = _chunk.read<uint8_t>(pos++);
		if (length & 0x80) {
			length = ((length & 0x7f) << 8) | _chunk.read<uint8_t>(pos++);
		}
		return length;
	};
	readLength();
	const uint32_t length = readLength();
	auto bytes = _chunk.sub(pos, length);
	return std::string(reinterpret_cast<const char*>(bytes.data()), bytes.size());
}

std::vector<ResStringPoolSpan> StringPool::styleAt(uint32_t index_) const {
	std::vector<ResStringPoolSpan> spans;
	if (index_ >= _header.styleCount) {
		return spans;
	}
	const size_t styleOffsets = _header.header.headerSize + static_cast<size_t>(_header.stringCount) * sizeof(uint32_t);
	size_t pos = _header.stylesStart + _chunk.read<uint32_t>(styleOffsets + index_ * sizeof(uint32_t));
	while (_chunk.read<uint32_t>(pos) != ResStringPoolSpan::END) {
		spans.push_back(_chunk.read<ResStringPoolSpan>(pos));
		pos += sizeof(ResStringPoolSpan);
	}
	return spans;
}
