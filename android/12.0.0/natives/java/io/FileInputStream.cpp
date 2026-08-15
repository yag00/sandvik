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
#include <sys/ioctl.h>

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
JNIEXPORT void JNICALL Java_java_io_FileInputStream_initIDs(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

	JNIEXPORT jint JNICALL Java_java_io_FileInputStream_available0(JNIEnv* env, jobject thiz) {
		auto thisObj = sandvik::native::getObject(thiz);
		auto fdObj = thisObj->getField("fd");
		int fd = fdObj->getField("descriptor")->getValue();

		int available = 0;
		if (::ioctl(fd, FIONREAD, &available) == -1) {
			off_t cur = ::lseek(fd, 0, SEEK_CUR);
			off_t end = ::lseek(fd, 0, SEEK_END);
			if (cur == static_cast<off_t>(-1) || end == static_cast<off_t>(-1)) {
				return 0;
			}
			::lseek(fd, cur, SEEK_SET);
			available = static_cast<int>(end - cur);
		}
		return static_cast<jint>(available);
	}

}  // extern "C"
