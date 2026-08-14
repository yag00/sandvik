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
#include <sys/stat.h>

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

extern "C" {

#if 0
JNIEXPORT void JNICALL Java_java_io_FileDescriptor_initIDs(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

	JNIEXPORT jboolean JNICALL Java_java_io_FileDescriptor_isSocket(JNIEnv* env, jobject obj) {
		if (obj == nullptr) {
			return JNI_FALSE;
		}

		jclass clazz = env->GetObjectClass(obj);
		jfieldID fid = env->GetFieldID(clazz, "descriptor", "I");

		if (fid == nullptr) {
			return JNI_FALSE;
		}

		jint fd = env->GetIntField(obj, fid);

		if (fd < 0) {
			return JNI_FALSE;
		}

		struct stat st;
		if (fstat(fd, &st) != 0) {
			return JNI_FALSE;
		}

		return S_ISSOCK(st.st_mode) ? JNI_TRUE : JNI_FALSE;
	}

}  // extern "C"
