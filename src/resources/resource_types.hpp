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

#ifndef __RESOURCES_RESOURCE_TYPES_HPP__
#define __RESOURCES_RESOURCE_TYPES_HPP__

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "exceptions.hpp"

/**
 * Binary layout of compiled Android resources (resources.arsc and binary XML), as defined by
 * frameworks/base/libs/androidfw/include/androidfw/ResourceTypes.h. All values are little endian.
 */
namespace sandvik::resources {
	/** @brief Chunk types (ResChunk_header::type) */
	enum ChunkType : uint16_t {
		RES_NULL_TYPE = 0x0000,
		RES_STRING_POOL_TYPE = 0x0001,
		RES_TABLE_TYPE = 0x0002,
		RES_XML_TYPE = 0x0003,
		RES_XML_START_NAMESPACE_TYPE = 0x0100,
		RES_XML_END_NAMESPACE_TYPE = 0x0101,
		RES_XML_START_ELEMENT_TYPE = 0x0102,
		RES_XML_END_ELEMENT_TYPE = 0x0103,
		RES_XML_CDATA_TYPE = 0x0104,
		RES_XML_RESOURCE_MAP_TYPE = 0x0180,
		RES_TABLE_PACKAGE_TYPE = 0x0200,
		RES_TABLE_TYPE_TYPE = 0x0201,
		RES_TABLE_TYPE_SPEC_TYPE = 0x0202,
		RES_TABLE_LIBRARY_TYPE = 0x0203,
		RES_TABLE_OVERLAYABLE_TYPE = 0x0204,
		RES_TABLE_OVERLAYABLE_POLICY_TYPE = 0x0205,
		RES_TABLE_STAGED_ALIAS_TYPE = 0x0206,
	};

	/** @brief Header common to every chunk */
	struct ResChunkHeader {
			uint16_t type;
			uint16_t headerSize;
			uint32_t size;
	};

	/** @brief Res_value::dataType */
	enum ValueType : uint8_t {
		TYPE_NULL = 0x00,
		TYPE_REFERENCE = 0x01,
		TYPE_ATTRIBUTE = 0x02,
		TYPE_STRING = 0x03,
		TYPE_FLOAT = 0x04,
		TYPE_DIMENSION = 0x05,
		TYPE_FRACTION = 0x06,
		TYPE_DYNAMIC_REFERENCE = 0x07,
		TYPE_DYNAMIC_ATTRIBUTE = 0x08,
		TYPE_INT_DEC = 0x10,
		TYPE_INT_HEX = 0x11,
		TYPE_INT_BOOLEAN = 0x12,
		TYPE_INT_COLOR_ARGB8 = 0x1c,
		TYPE_INT_COLOR_RGB8 = 0x1d,
		TYPE_INT_COLOR_ARGB4 = 0x1e,
		TYPE_INT_COLOR_RGB4 = 0x1f,
	};

	/** @brief Res_value::data of a TYPE_NULL value */
	constexpr uint32_t DATA_NULL_UNDEFINED = 0;
	constexpr uint32_t DATA_NULL_EMPTY = 1;

	/** @brief A typed value (Res_value) */
	struct ResValue {
			uint16_t size = sizeof(ResValue);
			uint8_t res0 = 0;
			uint8_t dataType = TYPE_NULL;
			uint32_t data = DATA_NULL_UNDEFINED;
	};

	/** @brief ResStringPool_header::flags */
	constexpr uint32_t STRING_POOL_SORTED_FLAG = 1 << 0;
	constexpr uint32_t STRING_POOL_UTF8_FLAG = 1 << 8;

	struct ResStringPoolHeader {
			ResChunkHeader header;
			uint32_t stringCount;
			uint32_t styleCount;
			uint32_t flags;
			uint32_t stringsStart;
			uint32_t stylesStart;
	};

	/** @brief One styled range of a string: name is the tag string index ("b", "i", ...) */
	struct ResStringPoolSpan {
			static constexpr uint32_t END = 0xffffffff;
			uint32_t name;
			uint32_t firstChar;
			uint32_t lastChar;
	};

	struct ResTableHeader {
			ResChunkHeader header;
			uint32_t packageCount;
	};

	struct ResTablePackage {
			ResChunkHeader header;
			uint32_t id;
			char16_t name[128];
			uint32_t typeStrings;
			uint32_t lastPublicType;
			uint32_t keyStrings;
			uint32_t lastPublicKey;
			uint32_t typeIdOffset;
	};

	struct ResTableTypeSpec {
			ResChunkHeader header;
			uint8_t id;
			uint8_t res0;
			uint16_t typesCount;
			uint32_t entryCount;
	};

	/** @brief ResTable_type::flags */
	constexpr uint8_t TYPE_FLAG_SPARSE = 0x01;
	/** @brief Offset of a missing entry in a dense ResTable_type */
	constexpr uint32_t NO_ENTRY = 0xffffffff;

	/** @brief ResTable_entry::flags */
	constexpr uint16_t ENTRY_FLAG_COMPLEX = 0x0001;
	constexpr uint16_t ENTRY_FLAG_PUBLIC = 0x0002;
	constexpr uint16_t ENTRY_FLAG_WEAK = 0x0004;

	struct ResTableEntry {
			uint16_t size;
			uint16_t flags;
			uint32_t key;
	};

	/** @brief Header of a complex entry (bag: style, array, plurals, attr...) */
	struct ResTableMapEntry {
			ResTableEntry entry;
			uint32_t parent;
			uint32_t count;
	};

	/** @brief One bag item: an attribute (or array index/plural key) and its value */
	struct ResTableMap {
			uint32_t name;
			ResValue value;
	};

	/** @brief Entry of a sparse ResTable_type: offset is in 4 bytes units */
	struct ResTableSparseTypeEntry {
			uint16_t idx;
			uint16_t offset;
	};

	struct ResXMLTreeNode {
			ResChunkHeader header;
			uint32_t lineNumber;
			uint32_t comment;
	};

	struct ResXMLTreeNamespaceExt {
			uint32_t prefix;
			uint32_t uri;
	};

	struct ResXMLTreeAttrExt {
			uint32_t ns;
			uint32_t name;
			uint16_t attributeStart;
			uint16_t attributeSize;
			uint16_t attributeCount;
			uint16_t idIndex;  // 1 based, 0 if none
			uint16_t classIndex;
			uint16_t styleIndex;
	};

	struct ResXMLTreeEndElementExt {
			uint32_t ns;
			uint32_t name;
	};

	struct ResXMLTreeAttribute {
			uint32_t ns;
			uint32_t name;
			uint32_t rawValue;
			ResValue typedValue;
	};

	struct ResXMLTreeCdataExt {
			uint32_t data;
			ResValue typedData;
	};

	/** @brief Index meaning "no string" in a string pool reference */
	constexpr uint32_t NO_STRING = 0xffffffff;

	/** @brief Resource id helpers: 0xPPTTEEEE (package, type, entry) */
	constexpr uint8_t getPackageId(uint32_t resid_) {
		return static_cast<uint8_t>(resid_ >> 24);
	}
	constexpr uint8_t getTypeId(uint32_t resid_) {
		return static_cast<uint8_t>(resid_ >> 16);
	}
	constexpr uint16_t getEntryId(uint32_t resid_) {
		return static_cast<uint16_t>(resid_);
	}
	constexpr uint32_t makeResId(uint8_t package_, uint8_t type_, uint16_t entry_) {
		return (static_cast<uint32_t>(package_) << 24) | (static_cast<uint32_t>(type_) << 16) | entry_;
	}
	/** @brief A valid resource id has non zero package and type ids */
	constexpr bool isValidResId(uint32_t resid_) {
		return (resid_ & 0xffff0000) != 0 && (resid_ & 0x00ff0000) != 0;
	}

	/**
	 * @brief Bounds checked reader over a byte buffer: compiled resources come from files and are never trusted.
	 */
	class ByteView {
		public:
			ByteView() = default;
			ByteView(const uint8_t* data_, size_t size_) : _data(data_), _size(size_) {
			}
			const uint8_t* data() const {
				return _data;
			}
			size_t size() const {
				return _size;
			}
			/** @brief Sub view [offset, offset + size[ */
			ByteView sub(size_t offset_, size_t size_) const {
				if (offset_ > _size || size_ > _size - offset_) {
					throw VmException("resources: truncated data (offset {} size {} in {} bytes)", offset_, size_, _size);
				}
				return ByteView(_data + offset_, size_);
			}
			/** @brief Reads a structure at offset (copied, no alignment requirement) */
			template <typename T>
			T read(size_t offset_) const {
				T value;
				std::memcpy(&value, sub(offset_, sizeof(T)).data(), sizeof(T));
				return value;
			}
			/** @brief Reads a structure whose stored header may be shorter than T (older formats): missing fields are zero */
			template <typename T>
			T readPartial(size_t offset_, size_t storedSize_) const {
				// value-initialization zeroes the fields (T may have default member initializers, so no memset)
				T value{};
				const size_t size = storedSize_ < sizeof(T) ? storedSize_ : sizeof(T);
				std::memcpy(&value, sub(offset_, size).data(), size);
				return value;
			}

		private:
			const uint8_t* _data = nullptr;
			size_t _size = 0;
	};
}  // namespace sandvik::resources

#endif  // __RESOURCES_RESOURCE_TYPES_HPP__
