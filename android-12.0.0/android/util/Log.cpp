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
#include <fmt/format.h>

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

extern "C" {
	jboolean Java_android_util_Log_isLoggable(JNIEnv* env, jclass clazz, jstring tag, jint level) {
		(void)clazz;

		auto sTag = sandvik::native::getString(tag);
		logger.fdebug("Log.isLoggable called with tag '{}' and level {}", sTag->str(), level);
		// Android log levels: VERBOSE(2) .. ASSERT(7)
		// return (level >= 2 && level <= 7) ? JNI_TRUE : JNI_FALSE;
		return JNI_TRUE;
	}
}  // extern "C"