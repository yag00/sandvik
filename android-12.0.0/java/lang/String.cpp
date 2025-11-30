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
	JNIEXPORT jchar JNICALL Java_java_lang_String_charAt(JNIEnv* env, jobject obj, jint index) {
		auto this_ptr = sandvik::native::getString(obj);
		const auto& str = this_ptr->str();

		size_t idx = static_cast<size_t>(index);
		if (idx >= str.size()) {
			throw sandvik::IndexOutOfBoundsException("Index out of bounds in String.charAt");
		}
		return static_cast<jchar>(str[idx]);
	}

	JNIEXPORT void JNICALL Java_java_lang_String_getCharsNoCheck(JNIEnv* env, jobject obj, jint start, jint end, jcharArray buffer, jint index) {
		auto this_ptr = sandvik::native::getString(obj);
		const auto& str = this_ptr->str();
		auto copyLen = end - start;

		if (copyLen > 0) {
			if (start < 0 || end > static_cast<jint>(str.size()) || start > end) {
				throw sandvik::IndexOutOfBoundsException("Invalid range in String.getCharsNoCheck");
			}
			std::vector<jchar> buf(copyLen);
			for (jsize i = 0; i < copyLen; ++i) {
				buf[i] = static_cast<jchar>(str[static_cast<size_t>(start + i)]);
			}
			env->SetCharArrayRegion(buffer, index, copyLen, buf.data());
		}
	}

	JNIEXPORT jint JNICALL Java_java_lang_String_compareTo(JNIEnv* env, jobject obj, jobject anotherString) {
		auto this_ptr = sandvik::native::getString(obj);
		auto other_ptr = sandvik::native::getString(anotherString);

		const auto& str1 = this_ptr->str();
		const auto& str2 = other_ptr->str();

		size_t minLen = std::min(str1.size(), str2.size());
		for (size_t i = 0; i < minLen; ++i) {
			if (str1[i] != str2[i]) {
				return static_cast<jint>(static_cast<int>(str1[i]) - static_cast<int>(str2[i]));
			}
		}
		return static_cast<jint>(static_cast<int>(str1.size()) - static_cast<int>(str2.size()));
	}

	JNIEXPORT jobject JNICALL Java_java_lang_String_fastSubstring(JNIEnv* env, jobject obj, jint start, jint length) {
		auto this_ptr = sandvik::native::getString(obj);
		const auto& str = this_ptr->str();

		if (start < 0 || length < 0 || static_cast<size_t>(start + length) > str.size()) {
			throw sandvik::IndexOutOfBoundsException("Invalid range in String.fastSubstring");
		}

		std::string substr = str.substr(static_cast<size_t>(start), static_cast<size_t>(length));
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		return (jobject)sandvik::Object::make(classloader, substr);
	}

	JNIEXPORT jobject JNICALL Java_java_lang_String_concat(JNIEnv* env, jobject obj, jobject strObj) {
		auto this_ptr = sandvik::native::getString(obj);
		auto arg_ptr = sandvik::native::getString(strObj);

		if (!arg_ptr || arg_ptr->str().empty()) {
			// If argument is null or empty, return this string
			return obj;
		}

		const auto& str1 = this_ptr->str();
		const auto& str2 = arg_ptr->str();

		std::string result = str1 + str2;
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		return (jobject)sandvik::Object::make(classloader, result);
	}

	JNIEXPORT jobject JNICALL Java_java_lang_String_doReplace(JNIEnv* env, jobject obj, jchar oldChar, jchar newChar) {
		auto this_ptr = sandvik::native::getString(obj);
		const auto& str = this_ptr->str();

		std::string replaced = str;
		for (auto& ch : replaced) {
			if (static_cast<jchar>(ch) == oldChar) {
				ch = static_cast<char>(newChar);
			}
		}
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		return (jobject)sandvik::Object::make(classloader, replaced);
	}

	JNIEXPORT jcharArray JNICALL Java_java_lang_String_toCharArray(JNIEnv* env, jobject obj) {
		auto this_ptr = sandvik::native::getString(obj);
		const auto& str = this_ptr->str();

		jsize len = static_cast<jsize>(str.size());
		jcharArray result = env->NewCharArray(len);
		if (!result) {
			throw sandvik::OutOfMemoryError("Failed to allocate char array in String.toCharArray");
		}
		if (len > 0) {
			std::vector<jchar> buf(len);
			for (jsize i = 0; i < len; ++i) {
				buf[i] = static_cast<jchar>(str[static_cast<size_t>(i)]);
			}
			env->SetCharArrayRegion(result, 0, len, buf.data());
		}
		return result;
	}

	JNIEXPORT jint JNICALL Java_java_lang_String_length(JNIEnv* env, jobject obj) {
		auto this_ptr = sandvik::native::getString(obj);
		const auto& str = this_ptr->str();
		return static_cast<jint>(str.size());
	}

	JNIEXPORT jboolean JNICALL Java_java_lang_String_isEmpty(JNIEnv* env, jobject obj) {
		auto this_ptr = sandvik::native::getString(obj);
		return this_ptr->str().empty();
	}

	JNIEXPORT jobject JNICALL Java_java_lang_String_intern(JNIEnv* env, jobject obj) {
		throw sandvik::VmException("Java_java_lang_String_intern not implemented!");
		/*auto this_ptr = sandvik::native::getString(obj);
		const auto& str = this_ptr->str();

		// Intern pool is managed by sandvik::native::StringPool
		sandvik::native::StringPool& pool = sandvik::native::getStringPool();
		jobject interned = pool.intern(env, obj, str);

		return interned;*/
	}

}  // extern "C"
