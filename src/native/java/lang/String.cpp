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

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
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

	JNIEXPORT jobject JNICALL Java_java_lang_String_substring__I(JNIEnv* env, jobject obj, jint beginIndex) {
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		auto this_ptr = sandvik::native::getString(obj);
		auto str = this_ptr->str();

		if (beginIndex < 0 || static_cast<size_t>(beginIndex) > str.size()) {
			throw sandvik::StringIndexOutOfBoundsException("beginIndex out of range");
		}

		auto substr = str.substr(beginIndex);
		auto strObj = sandvik::Object::make(classloader, substr);
		jobject jstr = jenv->getHandles().toJObject(strObj);
		return jstr;
	}

	JNIEXPORT jint JNICALL Java_java_lang_String_hashCode(JNIEnv* env, jobject obj) {
		auto this_ptr = sandvik::native::getString(obj);
		const auto& str = this_ptr->str();
		jint hash = 0;
		for (char c : str) {
			hash = 31 * hash + static_cast<unsigned char>(c);
		}
		return hash;
	}

	JNIEXPORT jobject JNICALL Java_java_lang_String_intern(JNIEnv* env, jobject obj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		auto this_ptr = sandvik::native::getString(obj);

		auto strObj = sandvik::Object::make(classloader, this_ptr->str());
		jobject jstr = jenv->getHandles().toJObject(strObj);
		return jstr;
	}

	JNIEXPORT jcharArray JNICALL Java_java_lang_String_toCharArray(JNIEnv* env, jobject obj) {
		auto this_ptr = sandvik::native::getString(obj);
		const auto& str = this_ptr->str();
		jsize len = static_cast<jsize>(str.size());

		jcharArray arr = env->NewCharArray(len);
		if (arr == nullptr) {
			throw sandvik::OutOfMemoryError("Failed to allocate char array");
		}

		if (len > 0) {
			std::vector<jchar> buf;
			buf.reserve(len);
			for (unsigned char c : str) {
				buf.push_back(static_cast<jchar>(c));
			}
			env->SetCharArrayRegion(arr, 0, len, buf.data());
		}

		return arr;
	}

	JNIEXPORT jint JNICALL Java_java_lang_String_length(JNIEnv* env, jobject obj) {
		auto this_ptr = sandvik::native::getString(obj);
		const auto& str = this_ptr->str();
		return static_cast<jint>(str.size());
	}

	JNIEXPORT void JNICALL Java_java_lang_String_initialize___3CII(JNIEnv* env, jobject obj, jcharArray chars, jint offset, jint count) {
		auto this_ptr = sandvik::native::getString(obj);
		auto array = sandvik::native::getArray(chars);
		auto arrLen = array->getArrayLength();

		if (offset < 0 || count < 0 || static_cast<jsize>(offset) + static_cast<jsize>(count) > arrLen) {
			throw sandvik::StringIndexOutOfBoundsException("offset/count out of range");
		}

		if (count == 0) {
			this_ptr->setString(std::string());
			return;
		}

		std::vector<jchar> buf(static_cast<size_t>(count));
		env->GetCharArrayRegion(chars, offset, count, buf.data());

		std::string s;
		s.reserve(static_cast<size_t>(count));
		for (jsize i = 0; i < count; ++i) {
			s.push_back(static_cast<char>(static_cast<unsigned char>(buf[static_cast<size_t>(i)] & 0xFF)));
		}
		this_ptr->setString(std::move(s));
	}

	JNIEXPORT void JNICALL Java_java_lang_String_getChars__II_3CI(JNIEnv* env, jobject obj, jint srcBegin, jint srcEnd, jcharArray dst, jint dstBegin) {
		logger.ferror("String.getChars(srcBegin={}, srcEnd={}, dst={}, dstBegin={})", srcBegin, srcEnd, (void*)dst, dstBegin);
		auto this_ptr = sandvik::native::getString(obj);
		const auto& str = this_ptr->str();
		logger.ferror("String value: '{}'", str);
		const auto str_len = str.size();

		// Validate source indices
		if (srcBegin < 0 || srcEnd < 0 || srcBegin > srcEnd || srcEnd > str_len) {
			throw sandvik::StringIndexOutOfBoundsException("srcBegin/srcEnd out of range");
		}
		// Validate destination array
		if (dst == nullptr) {
			throw sandvik::NullPointerException("destination char array is null");
		}

		auto copyLen = srcEnd - srcBegin;
		auto dstLen = env->GetArrayLength(dst);
		if (dstBegin < 0 || copyLen > dstLen - dstBegin) {
			throw sandvik::IndexOutOfBoundsException("destination index out of range");
		}

		if (copyLen > 0) {
			std::vector<jchar> buf;
			buf.reserve(copyLen);
			for (jsize i = 0; i < copyLen; ++i) {
				buf.push_back(static_cast<jchar>(static_cast<unsigned char>(str[static_cast<size_t>(srcBegin + i)])));
			}
			env->SetCharArrayRegion(dst, dstBegin, copyLen, buf.data());
		}
	}

	JNIEXPORT jint JNICALL Java_java_lang_String_codePointAt(JNIEnv* env, jobject obj, jint index) {
		auto this_ptr = sandvik::native::getString(obj);
		const auto& str = this_ptr->str();
		jsize len = static_cast<jsize>(str.size());

		if (index < 0 || index >= len) {
			throw sandvik::StringIndexOutOfBoundsException("index out of range");
		}

		// Retrieve jchar value stored in the native string (stored as low byte)
		jchar ch = static_cast<jchar>(static_cast<unsigned char>(str[static_cast<size_t>(index)]));

		// If high surrogate and next char is low surrogate, compose supplementary code point
		if (ch >= 0xD800 && ch <= 0xDBFF) {
			if (index + 1 < len) {
				jchar ch2 = static_cast<jchar>(static_cast<unsigned char>(str[static_cast<size_t>(index + 1)]));
				if (ch2 >= 0xDC00 && ch2 <= 0xDFFF) {
					jint hi = static_cast<jint>(ch) - 0xD800;
					jint lo = static_cast<jint>(ch2) - 0xDC00;
					return ((hi << 10) | lo) + 0x10000;
				}
			}
		}

		return static_cast<jint>(ch);
	}

	JNIEXPORT jchar JNICALL Java_java_lang_String_charAt(JNIEnv* env, jobject obj, jint index) {
		auto this_ptr = sandvik::native::getString(obj);
		const auto& str = this_ptr->str();
		jsize len = static_cast<jsize>(str.size());

		if (index < 0 || index >= len) {
			throw sandvik::StringIndexOutOfBoundsException("index out of range");
		}

		return static_cast<jchar>(static_cast<unsigned char>(str[static_cast<size_t>(index)]));
	}

	JNIEXPORT jobject JNICALL Java_java_lang_String_subSequence(JNIEnv* env, jobject obj, jint beginIndex, jint endIndex) {
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		auto this_ptr = sandvik::native::getString(obj);
		const auto& str = this_ptr->str();
		jsize len = static_cast<jsize>(str.size());

		if (beginIndex < 0 || endIndex < 0 || beginIndex > endIndex || endIndex > len) {
			throw sandvik::StringIndexOutOfBoundsException("beginIndex/endIndex out of range");
		}

		auto substr = str.substr(static_cast<size_t>(beginIndex), static_cast<size_t>(endIndex - beginIndex));
		auto strObj = sandvik::Object::make(classloader, substr);
		jobject jstr = jenv->getHandles().toJObject(strObj);
		return jstr;
	}

	JNIEXPORT jstring JNICALL Java_java_lang_String_toString(JNIEnv* env, jobject obj) {
		// java.lang.String.toString() simply returns this
		return static_cast<jstring>(obj);
	}
}