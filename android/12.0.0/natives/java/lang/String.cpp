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

#include <algorithm>

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "stringpool.hpp"
#include "system/logger.hpp"

/** @todo implementation **/

extern "C" {
	// Java strings are UTF-16: indexes and lengths are in UTF-16 code units (str16)

	JNIEXPORT jchar JNICALL Java_java_lang_String_charAt(JNIEnv* env, jobject obj, jint index) {
		const auto str = sandvik::native::getString(obj)->str16();
		if (index < 0 || static_cast<size_t>(index) >= str.size()) {
			throw sandvik::StringIndexOutOfBoundsException(fmt::format("index={} length={}", index, str.size()));
		}
		return static_cast<jchar>(str[static_cast<size_t>(index)]);
	}

	JNIEXPORT void JNICALL Java_java_lang_String_getCharsNoCheck(JNIEnv* env, jobject obj, jint start, jint end, jcharArray buffer, jint index) {
		const auto str = sandvik::native::getString(obj)->str16();
		const auto copyLen = end - start;
		if (copyLen > 0) {
			if (start < 0 || end > static_cast<jint>(str.size()) || start > end) {
				throw sandvik::IndexOutOfBoundsException("Invalid range in String.getCharsNoCheck");
			}
			env->SetCharArrayRegion(buffer, index, copyLen, reinterpret_cast<const jchar*>(str.data() + start));
		}
	}

	JNIEXPORT jint JNICALL Java_java_lang_String_compareTo(JNIEnv* env, jobject obj, jobject anotherString) {
		const auto str1 = sandvik::native::getString(obj)->str16();
		const auto str2 = sandvik::native::getString(anotherString)->str16();
		const size_t minLen = std::min(str1.size(), str2.size());
		for (size_t i = 0; i < minLen; ++i) {
			if (str1[i] != str2[i]) {
				return static_cast<jint>(str1[i]) - static_cast<jint>(str2[i]);
			}
		}
		return static_cast<jint>(str1.size()) - static_cast<jint>(str2.size());
	}

	JNIEXPORT jint JNICALL Java_java_lang_String_compareTo__Ljava_lang_String_2(JNIEnv* env, jobject obj, jstring anotherString) {
		return Java_java_lang_String_compareTo(env, obj, anotherString);
	}

	JNIEXPORT jobject JNICALL Java_java_lang_String_fastSubstring(JNIEnv* env, jobject obj, jint start, jint length) {
		const auto str = sandvik::native::getString(obj)->str16();
		if (start < 0 || length < 0 || static_cast<size_t>(start) + static_cast<size_t>(length) > str.size()) {
			throw sandvik::IndexOutOfBoundsException("Invalid range in String.fastSubstring");
		}
		auto& classloader = sandvik::native::getNativeInterface(env)->getClassLoader();
		return (jobject)sandvik::Object::make(classloader, str.substr(static_cast<size_t>(start), static_cast<size_t>(length)));
	}

	JNIEXPORT jobject JNICALL Java_java_lang_String_concat(JNIEnv* env, jobject obj, jobject strObj) {
		const auto str2 = sandvik::native::getString(strObj)->str16();
		if (str2.empty()) {
			return obj;
		}
		auto& classloader = sandvik::native::getNativeInterface(env)->getClassLoader();
		return (jobject)sandvik::Object::make(classloader, sandvik::native::getString(obj)->str16() + str2);
	}

	JNIEXPORT jobject JNICALL Java_java_lang_String_doReplace(JNIEnv* env, jobject obj, jchar oldChar, jchar newChar) {
		auto replaced = sandvik::native::getString(obj)->str16();
		std::replace(replaced.begin(), replaced.end(), static_cast<char16_t>(oldChar), static_cast<char16_t>(newChar));
		auto& classloader = sandvik::native::getNativeInterface(env)->getClassLoader();
		return (jobject)sandvik::Object::make(classloader, replaced);
	}

	JNIEXPORT jcharArray JNICALL Java_java_lang_String_toCharArray(JNIEnv* env, jobject obj) {
		const auto str = sandvik::native::getString(obj)->str16();
		const auto len = static_cast<jsize>(str.size());
		jcharArray result = env->NewCharArray(len);
		if (!result) {
			throw sandvik::OutOfMemoryError("Failed to allocate char array in String.toCharArray");
		}
		if (len > 0) {
			env->SetCharArrayRegion(result, 0, len, reinterpret_cast<const jchar*>(str.data()));
		}
		return result;
	}

	JNIEXPORT jint JNICALL Java_java_lang_String_length(JNIEnv* env, jobject obj) {
		return static_cast<jint>(sandvik::native::getString(obj)->str16().size());
	}

	JNIEXPORT jboolean JNICALL Java_java_lang_String_isEmpty(JNIEnv* env, jobject obj) {
		auto this_ptr = sandvik::native::getString(obj);
		return this_ptr->str().empty();
	}

	JNIEXPORT jobject JNICALL Java_java_lang_String_intern(JNIEnv* env, jobject obj) {
		auto this_ptr = sandvik::native::getString(obj);
		const auto& str = this_ptr->str();
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		auto& stringPool = sandvik::StringPool::getInstance();
		return (jobject)stringPool.intern(classloader, str);
	}

}  // extern "C"
