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

namespace {
	// Android log priorities: VERBOSE(2), DEBUG(3), INFO(4), WARN(5), ERROR(6), ASSERT(7)
	char toPriority(jint priority_) {
		switch (priority_) {
			case 2:
				return 'V';
			case 3:
				return 'D';
			case 5:
				return 'W';
			case 6:
				return 'E';
			case 7:
				return 'F';
			default:
				return 'I';
		}
	}

	Logger::LogLevel toLogLevel(jint priority_) {
		switch (toPriority(priority_)) {
			case 'V':
				return Logger::LogLevel::VERBOSE;
			case 'D':
				return Logger::LogLevel::DEBUG;
			case 'W':
				return Logger::LogLevel::WARNING;
			case 'E':
			case 'F':
				return Logger::LogLevel::ERROR;
			default:
				return Logger::LogLevel::INFO;
		}
	}
}  // namespace

extern "C" {
	jboolean Java_android_util_Log_isLoggable(JNIEnv* env, jclass clazz, jstring tag, jint level) {
		(void)env;
		(void)clazz;

		auto sTag = sandvik::native::getString(tag);
		logger.ftrace("Log.isLoggable called with tag '{}' and level {}", sTag->str(), level);
		return logger.isAppEnabled(toLogLevel(level)) ? JNI_TRUE : JNI_FALSE;
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
		logger.app(toPriority(priority), sTag->str(), sMsg->str());

		return static_cast<jint>(sMsg->str().size());
	}
}  // extern "C"