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

	JNIEXPORT jint JNICALL Java_android_util_Log_logger_entry_max_payload_native(JNIEnv* env, jclass clazz) {
		(void)env;
		(void)clazz;
		// Approximate linux logger payload max size used by framework log splitting heuristics.
		return 4068;
	}

	JNIEXPORT jint JNICALL Java_android_util_Log_println_native(JNIEnv* env, jclass clazz, jint bufID, jint priority, jstring tag, jstring msg) {
		(void)clazz;
		(void)bufID;

		auto sTag = sandvik::native::getString(tag);
		auto sMsg = sandvik::native::getString(msg);
		switch (priority) {
			case 2:
				logger.fdebug("[{}] {}", sTag->str(), sMsg->str());
				break;
			case 3:
				logger.finfo("[{}] {}", sTag->str(), sMsg->str());
				break;
			case 4:
				logger.finfo("[{}] {}", sTag->str(), sMsg->str());
				break;
			case 5:
				logger.fwarning("[{}] {}", sTag->str(), sMsg->str());
				break;
			case 6:
			case 7:
				logger.ferror("[{}] {}", sTag->str(), sMsg->str());
				break;
			default:
				logger.finfo("[{}] {}", sTag->str(), sMsg->str());
				break;
		}

		return static_cast<jint>(sMsg->str().size());
	}
}  // extern "C"