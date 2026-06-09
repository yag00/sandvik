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

#include <errno.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>

#include <fmt/format.h>
#include <sys/utsname.h>

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/env_var.hpp"
#include "system/logger.hpp"

using namespace sandvik;

extern "C" {
	JNIEXPORT void JNICALL Java_java_io_UnixFileSystem_initIDs(JNIEnv* env, jclass clazz) {
		// In OpenJDK/Android, this would compute field offsets for fast access
		// In our VM stub, we do nothing.
	}
}  // extern "C"
