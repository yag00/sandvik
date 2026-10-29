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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <unistd.h>

#include <fmt/format.h>
#include <jni/jni.h>
#include <sys/mman.h>

#include <cerrno>
#include <cstring>

#include "exceptions.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

namespace {
	// FileChannelImpl.MAP_* modes
	constexpr jint MAP_RO = 0;
	constexpr jint MAP_RW = 1;
	constexpr jint MAP_PV = 2;

	int getFd(jobject fileDescriptor_) {
		return (int)sandvik::native::getObject(fileDescriptor_)->getField("descriptor")->getValue();
	}
}  // namespace

extern "C" {

	// Returns the allocation granularity used to align mapping offsets
	JNIEXPORT jlong JNICALL Java_sun_nio_ch_FileChannelImpl_initIDs(JNIEnv*, jclass) {
		return sysconf(_SC_PAGESIZE);
	}

	JNIEXPORT jlong JNICALL Java_sun_nio_ch_FileChannelImpl_map0(JNIEnv*, jobject thiz, jint prot, jlong position, jlong length) {
		const int fd = getFd((jobject)sandvik::native::getObject(thiz)->getField("fd"));
		int protections = PROT_READ;
		int flags = MAP_SHARED;
		if (prot == MAP_RW) {
			protections = PROT_READ | PROT_WRITE;
		} else if (prot == MAP_PV) {
			protections = PROT_READ | PROT_WRITE;
			flags = MAP_PRIVATE;
		} else if (prot != MAP_RO) {
			throw IllegalArgumentException(fmt::format("FileChannelImpl.map0: invalid mode {}", prot));
		}
		void* address = mmap(nullptr, (size_t)length, protections, flags, fd, (off_t)position);
		if (address == MAP_FAILED) {
			throw IOException(fmt::format("map failed: {}", strerror(errno)));
		}
		return static_cast<jlong>(reinterpret_cast<uintptr_t>(address));
	}

	JNIEXPORT jint JNICALL Java_sun_nio_ch_FileChannelImpl_unmap0(JNIEnv*, jclass, jlong address, jlong length) {
		if (munmap(reinterpret_cast<void*>(static_cast<uintptr_t>(address)), (size_t)length) != 0) {
			throw IOException(fmt::format("unmap failed: {}", strerror(errno)));
		}
		return 0;
	}

	// offset < 0 queries the current position, otherwise moves to offset
	JNIEXPORT jlong JNICALL Java_sun_nio_ch_FileChannelImpl_position0(JNIEnv*, jobject, jobject fileDescriptor, jlong offset) {
		const int fd = getFd(fileDescriptor);
		const off_t result = offset < 0 ? lseek(fd, 0, SEEK_CUR) : lseek(fd, (off_t)offset, SEEK_SET);
		if (result < 0) {
			throw IOException(fmt::format("lseek failed: {}", strerror(errno)));
		}
		return result;
	}

	JNIEXPORT jlong JNICALL Java_sun_nio_ch_FileChannelImpl_transferTo0(JNIEnv*, jobject, jobject, jlong, jlong, jobject) {
		// IOStatus.UNSUPPORTED: FileChannelImpl falls back to a buffered copy
		return -4;
	}

}  // extern "C"
