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

#include <jni/jni.h>

#include "exceptions.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"

using namespace sandvik;

extern "C" {
	JNIEXPORT jstring JNICALL Java_java_lang_StringFactory_newStringFromChars__II_3C(JNIEnv* env, jclass clazz, jint offset, jint count, jcharArray value) {
		if (value == nullptr) {
			throw NullPointerException("Null char array in StringFactory.newStringFromChars");
		}
		jsize len = env->GetArrayLength(value);
		if (offset < 0 || count < 0 || offset + count > len) {
			throw IndexOutOfBoundsException("Invalid range in StringFactory.newStringFromChars");
		}
		std::vector<jchar> buf(count);
		env->GetCharArrayRegion(value, offset, count, buf.data());
		std::string str(buf.begin(), buf.end());
		auto jenv = native::getNativeInterface(env);
		auto& classloader = jenv->getClassLoader();
		return (jstring)sandvik::Object::make(classloader, str);
	}

	JNIEXPORT jstring JNICALL Java_java_lang_StringFactory_newStringFromUtf8Bytes(JNIEnv* env, jclass clazz, jbyteArray bytes) {
		if (bytes == nullptr) {
			throw NullPointerException("Null byte array in StringFactory.newStringFromUtf8Bytes");
		}
		jsize len = env->GetArrayLength(bytes);
		std::vector<jbyte> buf(len);
		env->GetByteArrayRegion(bytes, 0, len, buf.data());
		std::string str(buf.begin(), buf.end());
		auto jenv = native::getNativeInterface(env);
		auto& classloader = jenv->getClassLoader();
		return (jstring)sandvik::Object::make(classloader, str);
	}
}  // extern "C"
