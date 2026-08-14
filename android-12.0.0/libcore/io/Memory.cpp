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

#include <algorithm>
#include <vector>

#include "array.hpp"
#include "exceptions.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

extern "C" {
	JNIEXPORT void JNICALL Java_libcore_io_Memory_unsafeBulkGet(JNIEnv* env, jclass clazz, jobject dstObj, jint dstOffset, jint byteCount,
	                                                            jbyteArray srcArrayObj, jint srcOffset, jint sizeofElement, jboolean swap) {
		if (!dstObj || !srcArrayObj) {
			throw NullPointerException("Null buffer in unsafeBulkGet");
		}
		auto srcArray = sandvik::native::getArray(srcArrayObj);
		auto dstArray = sandvik::native::getArray(dstObj);

		if (dstOffset < 0 || srcOffset < 0 || byteCount < 0) {
			throw IndexOutOfBoundsException("Negative offset or byteCount in unsafeBulkGet");
		}

		std::vector<uint8_t> bytes(byteCount);
		for (jint i = 0; i < byteCount; ++i) {
			bytes[i] = static_cast<uint8_t>(srcArray->getElement(srcOffset + i)->getValue());
		}

		if (swap) {
			if (sizeofElement == 2) {
				for (jint i = 0; i + 1 < byteCount; i += 2) {
					std::swap(bytes[i], bytes[i + 1]);
				}
			} else if (sizeofElement == 4) {
				for (jint i = 0; i + 3 < byteCount; i += 4) {
					std::swap(bytes[i], bytes[i + 3]);
					std::swap(bytes[i + 1], bytes[i + 2]);
				}
			} else if (sizeofElement == 8) {
				for (jint i = 0; i + 7 < byteCount; i += 8) {
					std::swap(bytes[i], bytes[i + 7]);
					std::swap(bytes[i + 1], bytes[i + 6]);
					std::swap(bytes[i + 2], bytes[i + 5]);
					std::swap(bytes[i + 3], bytes[i + 4]);
				}
			}
		}

		if (sizeofElement == 1) {
			for (jint i = 0; i < byteCount; ++i) {
				dstArray->setElement(dstOffset + i, Object::make(static_cast<int32_t>(static_cast<int8_t>(bytes[i]))));
			}
		} else if (sizeofElement == 2) {
			jint numElements = byteCount / 2;
			for (jint i = 0; i < numElements; ++i) {
				uint16_t val = static_cast<uint16_t>(bytes[2 * i]) | (static_cast<uint16_t>(bytes[2 * i + 1]) << 8);
				dstArray->setElement(dstOffset + i, Object::make(static_cast<int32_t>(val)));
			}
		} else if (sizeofElement == 4) {
			jint numElements = byteCount / 4;
			for (jint i = 0; i < numElements; ++i) {
				uint32_t val = static_cast<uint32_t>(bytes[4 * i]) | (static_cast<uint32_t>(bytes[4 * i + 1]) << 8) |
				               (static_cast<uint32_t>(bytes[4 * i + 2]) << 16) | (static_cast<uint32_t>(bytes[4 * i + 3]) << 24);
				dstArray->setElement(dstOffset + i, Object::make(static_cast<int32_t>(val)));
			}
		} else if (sizeofElement == 8) {
			jint numElements = byteCount / 8;
			for (jint i = 0; i < numElements; ++i) {
				uint64_t val = static_cast<uint64_t>(bytes[8 * i]) | (static_cast<uint64_t>(bytes[8 * i + 1]) << 8) |
				               (static_cast<uint64_t>(bytes[8 * i + 2]) << 16) | (static_cast<uint64_t>(bytes[8 * i + 3]) << 24) |
				               (static_cast<uint64_t>(bytes[8 * i + 4]) << 32) | (static_cast<uint64_t>(bytes[8 * i + 5]) << 40) |
				               (static_cast<uint64_t>(bytes[8 * i + 6]) << 48) | (static_cast<uint64_t>(bytes[8 * i + 7]) << 56);
				dstArray->setElement(dstOffset + i, Object::make(static_cast<int64_t>(val)));
			}
		}
	}

	JNIEXPORT void JNICALL Java_libcore_io_Memory_unsafeBulkPut(JNIEnv* env, jclass clazz, jbyteArray dstArrayObj, jint dstOffset, jint byteCount,
	                                                            jobject srcObj, jint srcOffset, jint sizeofElement, jboolean swap) {
		if (!dstArrayObj || !srcObj) {
			throw NullPointerException("Null buffer in unsafeBulkPut");
		}
		auto dstArray = sandvik::native::getArray(dstArrayObj);
		auto srcArray = sandvik::native::getArray(srcObj);

		if (dstOffset < 0 || srcOffset < 0 || byteCount < 0) {
			throw IndexOutOfBoundsException("Negative offset or byteCount in unsafeBulkPut");
		}

		std::vector<uint8_t> bytes(byteCount);
		if (sizeofElement == 1) {
			for (jint i = 0; i < byteCount; ++i) {
				bytes[i] = static_cast<uint8_t>(srcArray->getElement(srcOffset + i)->getValue());
			}
		} else if (sizeofElement == 2) {
			jint numElements = byteCount / 2;
			for (jint i = 0; i < numElements; ++i) {
				uint16_t val = static_cast<uint16_t>(srcArray->getElement(srcOffset + i)->getValue());
				bytes[2 * i] = static_cast<uint8_t>(val & 0xFF);
				bytes[2 * i + 1] = static_cast<uint8_t>((val >> 8) & 0xFF);
			}
		} else if (sizeofElement == 4) {
			jint numElements = byteCount / 4;
			for (jint i = 0; i < numElements; ++i) {
				uint32_t val = static_cast<uint32_t>(srcArray->getElement(srcOffset + i)->getValue());
				bytes[4 * i] = static_cast<uint8_t>(val & 0xFF);
				bytes[4 * i + 1] = static_cast<uint8_t>((val >> 8) & 0xFF);
				bytes[4 * i + 2] = static_cast<uint8_t>((val >> 16) & 0xFF);
				bytes[4 * i + 3] = static_cast<uint8_t>((val >> 24) & 0xFF);
			}
		} else if (sizeofElement == 8) {
			jint numElements = byteCount / 8;
			for (jint i = 0; i < numElements; ++i) {
				uint64_t val = static_cast<uint64_t>(srcArray->getElement(srcOffset + i)->getValue());
				bytes[8 * i] = static_cast<uint8_t>(val & 0xFF);
				bytes[8 * i + 1] = static_cast<uint8_t>((val >> 8) & 0xFF);
				bytes[8 * i + 2] = static_cast<uint8_t>((val >> 16) & 0xFF);
				bytes[8 * i + 3] = static_cast<uint8_t>((val >> 24) & 0xFF);
				bytes[8 * i + 4] = static_cast<uint8_t>((val >> 32) & 0xFF);
				bytes[8 * i + 5] = static_cast<uint8_t>((val >> 40) & 0xFF);
				bytes[8 * i + 6] = static_cast<uint8_t>((val >> 48) & 0xFF);
				bytes[8 * i + 7] = static_cast<uint8_t>((val >> 56) & 0xFF);
			}
		}

		if (swap) {
			if (sizeofElement == 2) {
				for (jint i = 0; i + 1 < byteCount; i += 2) {
					std::swap(bytes[i], bytes[i + 1]);
				}
			} else if (sizeofElement == 4) {
				for (jint i = 0; i + 3 < byteCount; i += 4) {
					std::swap(bytes[i], bytes[i + 3]);
					std::swap(bytes[i + 1], bytes[i + 2]);
				}
			} else if (sizeofElement == 8) {
				for (jint i = 0; i + 7 < byteCount; i += 8) {
					std::swap(bytes[i], bytes[i + 7]);
					std::swap(bytes[i + 1], bytes[i + 6]);
					std::swap(bytes[i + 2], bytes[i + 5]);
					std::swap(bytes[i + 3], bytes[i + 4]);
				}
			}
		}

		for (jint i = 0; i < byteCount; ++i) {
			dstArray->setElement(dstOffset + i, Object::make(static_cast<int32_t>(static_cast<int8_t>(bytes[i]))));
		}
	}
}  // extern "C"
