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

#include <fmt/format.h>
#include <jni/jni.h>
#include <sys/stat.h>

#include <cerrno>
#include <cstring>

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

/** @todo implementation **/

using namespace sandvik;

extern "C" {

	JNIEXPORT jlong JNICALL Java_sun_nio_ch_FileDispatcherImpl_size0(JNIEnv*, jclass, jobject fileDescriptor) {
		const int fd = (int)sandvik::native::getObject(fileDescriptor)->getField("descriptor")->getValue();
		struct stat st;
		if (fstat(fd, &st) != 0) {
			throw IOException(fmt::format("fstat failed: {}", strerror(errno)));
		}
		return st.st_size;
	}

#if 0
JNIEXPORT void JNICALL Java_sun_nio_ch_FileDispatcherImpl_init(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

}  // extern "C"
