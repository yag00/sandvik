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

#include "exceptions.hpp"

using namespace sandvik;

extern "C" {

	JNIEXPORT void JNICALL Java_libcore_util_CharsetUtils_asciiBytesToChars(JNIEnv* env, jclass, jbyteArray bytes, jint offset, jint length, jcharArray chars) {
		if (bytes == nullptr || chars == nullptr) {
			throw NullPointerException("bytes or chars is null");
			return;
		}

		jsize bytesLength = env->GetArrayLength(bytes);
		jsize charsLength = env->GetArrayLength(chars);
		if (offset < 0 || length < 0 || offset + length > bytesLength || length > charsLength) {
			throw ArrayIndexOutOfBoundsException("Invalid offset or length");
			return;
		}

		jbyte* srcBytes = static_cast<jbyte*>(env->GetByteArrayElements(bytes, nullptr));
		if (srcBytes == nullptr) {
			// An error occurred while accessing the byte array
			return;
		}

		// Get the char array elements
		jchar* dstChars = static_cast<jchar*>(env->GetCharArrayElements(chars, nullptr));
		if (dstChars == nullptr) {
			env->ReleaseByteArrayElements(bytes, srcBytes, JNI_ABORT);
			return;
		}

		// Perform the conversion from ASCII bytes to chars
		const uint8_t* src = reinterpret_cast<const uint8_t*>(srcBytes) + offset;
		for (jint i = 0; i < length; ++i) {
			dstChars[i] = static_cast<jchar>(src[i]);
		}

		env->ReleaseCharArrayElements(chars, dstChars, 0);
		env->ReleaseByteArrayElements(bytes, srcBytes, JNI_ABORT);
	}
}  // extern "C"
