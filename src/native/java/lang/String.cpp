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
#include <jni/jni.h>

#include "class.hpp"
#include "classloader.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "jnihandlemap.hpp"
#include "native/native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

extern "C" {
	JNIEXPORT void JNICALL Java_java_lang_String_initialize__Ljava_lang_String_2(JNIEnv* env, jobject obj, jobject other) {
		auto this_ptr = sandvik::native::getString(obj);
		auto other_ptr = sandvik::native::getString(other);
		this_ptr->setString(this_ptr->str() + other_ptr->str());
	}

	JNIEXPORT jboolean JNICALL Java_java_lang_String_equals(JNIEnv* env, jobject obj, jobject other) {
		auto this_ptr = sandvik::native::getString(obj);
		auto other_ptr = sandvik::native::getString(other);
		// Get the actual string values
		auto this_str = this_ptr->str();
		auto other_str = other_ptr->str();
		// Compare the strings for equality
		return this_str == other_str ? JNI_TRUE : JNI_FALSE;
	}
}