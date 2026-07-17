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

#include <jni/jni.h>

#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

extern "C" {

	// Resolves a system property key with a Java-provided default fallback.
	JNIEXPORT jstring JNICALL Java_android_os_SystemProperties_native_get__Ljava_lang_String_2Ljava_lang_String_2(JNIEnv* env, jclass clazz, jstring key,
	                                                                                                              jstring def) {
		(void)env;
		(void)clazz;

		auto keyStr = sandvik::native::getString(key)->str();
		logger.fwarning("SystemProperties.native_get called with key: {}, returning default value", keyStr);
		return def;
	}

}  // extern "C"
