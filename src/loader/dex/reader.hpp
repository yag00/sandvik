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

#ifndef __SANDVIK_DEX_READER_HPP__
#define __SANDVIK_DEX_READER_HPP__

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace sandvik {
	namespace dex {
		/** @brief Exception thrown when a DEX file is malformed or truncated. */
		class DexFormatException : public std::runtime_error {
				using std::runtime_error::runtime_error;
		};

		/** @brief Bounds-checked cursor over a raw DEX file buffer.
		 *
		 * All multi-byte integers in the DEX format are little-endian. ULEB128/SLEB128 are the
		 * variable-length integer encodings used throughout the format (counts, indices, offsets
		 * within encoded_{field,method} and code_item structures). MUTF-8 is Java's "Modified UTF-8"
		 * string encoding used by string_data_item.
		 */
		class Reader {
			public:
				/** @brief Construct a reader over a buffer it does not own.
				 * @param data_ Pointer to the start of the DEX file buffer.
				 * @param size_ Size of the buffer in bytes.
				 */
				Reader(const uint8_t* data_, size_t size_) : _data(data_), _size(size_) {
				}

				/** @brief Total size of the underlying buffer. */
				size_t size() const {
					return _size;
				}

				/** @brief Raw pointer to the underlying buffer. */
				const uint8_t* data() const {
					return _data;
				}

				/** @brief Throws if [offset_, offset_+len_) is not fully within the buffer. */
				void checkBounds(size_t offset_, size_t len_) const {
					if (offset_ > _size || len_ > _size - offset_) {
						throw DexFormatException("DEX read out of bounds");
					}
				}

				/** @brief Reads a single byte at a fixed offset (no cursor advance). */
				uint8_t u1(size_t offset_) const {
					checkBounds(offset_, 1);
					return _data[offset_];
				}
				/** @brief Reads a little-endian uint16 at a fixed offset (no cursor advance). */
				uint16_t u2(size_t offset_) const {
					checkBounds(offset_, 2);
					return static_cast<uint16_t>(_data[offset_]) | (static_cast<uint16_t>(_data[offset_ + 1]) << 8);
				}
				/** @brief Reads a little-endian uint32 at a fixed offset (no cursor advance). */
				uint32_t u4(size_t offset_) const {
					checkBounds(offset_, 4);
					return static_cast<uint32_t>(_data[offset_]) | (static_cast<uint32_t>(_data[offset_ + 1]) << 8) |
					       (static_cast<uint32_t>(_data[offset_ + 2]) << 16) | (static_cast<uint32_t>(_data[offset_ + 3]) << 24);
				}
				/** @brief Reads a little-endian uint64 at a fixed offset (no cursor advance). */
				uint64_t u8(size_t offset_) const {
					checkBounds(offset_, 8);
					uint64_t lo = u4(offset_);
					uint64_t hi = u4(offset_ + 4);
					return lo | (hi << 32);
				}

				/** @brief Reads a byte at cursor_, advancing it by 1. */
				uint8_t readU1(size_t& cursor_) const {
					uint8_t v = u1(cursor_);
					cursor_ += 1;
					return v;
				}
				/** @brief Reads a little-endian uint16 at cursor_, advancing it by 2. */
				uint16_t readU2(size_t& cursor_) const {
					uint16_t v = u2(cursor_);
					cursor_ += 2;
					return v;
				}
				/** @brief Reads a little-endian uint32 at cursor_, advancing it by 4. */
				uint32_t readU4(size_t& cursor_) const {
					uint32_t v = u4(cursor_);
					cursor_ += 4;
					return v;
				}
				/** @brief Reads a little-endian uint64 at cursor_, advancing it by 8. */
				uint64_t readU8(size_t& cursor_) const {
					uint64_t v = u8(cursor_);
					cursor_ += 8;
					return v;
				}

				/** @brief Reads an unsigned LEB128 value at cursor_, advancing it past the encoding. */
				uint32_t readULEB128(size_t& cursor_) const {
					uint32_t result = 0;
					int shift = 0;
					uint8_t byte;
					do {
						byte = readU1(cursor_);
						result |= static_cast<uint32_t>(byte & 0x7f) << shift;
						shift += 7;
						if (shift > 35) {
							throw DexFormatException("Malformed ULEB128 (too long)");
						}
					} while (byte & 0x80);
					return result;
				}

				/** @brief Reads a signed LEB128 value at cursor_, advancing it past the encoding. */
				int32_t readSLEB128(size_t& cursor_) const {
					int32_t result = 0;
					int shift = 0;
					uint8_t byte;
					do {
						byte = readU1(cursor_);
						result |= static_cast<int32_t>(byte & 0x7f) << shift;
						shift += 7;
						if (shift > 35) {
							throw DexFormatException("Malformed SLEB128 (too long)");
						}
					} while (byte & 0x80);
					// Sign-extend if the last byte's sign bit (bit 6) was set and we haven't filled all 32 bits.
					if (shift < 32 && (byte & 0x40)) {
						result |= -(static_cast<int32_t>(1) << shift);
					}
					return result;
				}

				/** @brief Decodes a string_data_item (uleb128 utf16_size followed by Modified-UTF-8 bytes,
				 * NUL-terminated) at the given offset into a standard UTF-8 std::string.
				 * @param offset_ Offset of the string_data_item within the buffer.
				 * @return Decoded string, re-encoded as standard UTF-8.
				 */
				std::string readMUTF8String(size_t offset_) const {
					size_t cursor = offset_;
					uint32_t utf16Size = readULEB128(cursor);
					std::string out;
					out.reserve(utf16Size);
					uint32_t decodedUnits = 0;
					while (decodedUnits < utf16Size) {
						uint8_t b0 = readU1(cursor);
						if (b0 == 0) {
							throw DexFormatException("Truncated MUTF-8 string data");
						}
						uint32_t codepoint = 0;
						if ((b0 & 0x80) == 0) {
							// 1-byte sequence: 0xxxxxxx
							codepoint = b0;
							decodedUnits += 1;
						} else if ((b0 & 0xE0) == 0xC0) {
							// 2-byte sequence: 110xxxxx 10xxxxxx (also encodes embedded NUL as C0 80)
							uint8_t b1 = readU1(cursor);
							codepoint = (static_cast<uint32_t>(b0 & 0x1F) << 6) | (b1 & 0x3F);
							decodedUnits += 1;
						} else if ((b0 & 0xF0) == 0xE0) {
							// 3-byte sequence: 1110xxxx 10xxxxxx 10xxxxxx - includes CESU-8 surrogate halves
							uint8_t b1 = readU1(cursor);
							uint8_t b2 = readU1(cursor);
							codepoint = (static_cast<uint32_t>(b0 & 0x0F) << 12) | (static_cast<uint32_t>(b1 & 0x3F) << 6) | (b2 & 0x3F);
							decodedUnits += 1;
						} else {
							throw DexFormatException("Invalid MUTF-8 leading byte");
						}
						appendUtf8Codepoint(out, codepoint);
					}
					return out;
				}

			private:
				/** @brief Appends one Unicode codepoint (already decoded from MUTF-8/CESU-8, so
				 * surrogate halves arrive as separate calls) to a std::string as standard UTF-8.
				 * Since CESU-8 surrogate pairs decode to two separate 16-bit codepoints rather than
				 * one combined one, each is re-encoded independently; the result is bit-for-bit the
				 * CESU-8/WTF-8 style encoding for those (rare, non-BMP) characters, which every
				 * consumer in this codebase treats as an opaque UTF-8-ish string, not something
				 * requiring codepoint-level correctness.
				 */
				static void appendUtf8Codepoint(std::string& out_, uint32_t codepoint_) {
					if (codepoint_ <= 0x7F) {
						out_ += static_cast<char>(codepoint_);
					} else if (codepoint_ <= 0x7FF) {
						out_ += static_cast<char>(0xC0 | (codepoint_ >> 6));
						out_ += static_cast<char>(0x80 | (codepoint_ & 0x3F));
					} else {
						out_ += static_cast<char>(0xE0 | (codepoint_ >> 12));
						out_ += static_cast<char>(0x80 | ((codepoint_ >> 6) & 0x3F));
						out_ += static_cast<char>(0x80 | (codepoint_ & 0x3F));
					}
				}

				const uint8_t* _data;
				size_t _size;
		};
	}  // namespace dex
}  // namespace sandvik

#endif  // __SANDVIK_DEX_READER_HPP__
