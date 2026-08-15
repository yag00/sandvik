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
#include "field.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/env_var.hpp"
#include "system/logger.hpp"

using namespace sandvik;

extern "C" {
	JNIEXPORT jstring JNICALL Java_libcore_icu_ICU_getIcuVersion(JNIEnv* env, jclass) {
		//@todo implement proper ICU version retrieval
		return env->NewStringUTF("50.1.1");
	}
	JNIEXPORT jstring JNICALL Java_libcore_icu_ICU_getUnicodeVersion(JNIEnv* env, jclass) {
		//@todo implement proper Unicode version retrieval
		return env->NewStringUTF("6.2");
	}
	JNIEXPORT jstring JNICALL Java_libcore_icu_ICU_getCldrVersion(JNIEnv* env, jclass) {
		//@todo implement proper CLDR version retrieval
		return env->NewStringUTF("22.1.1");
	}
}  // extern "C"
