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

#include "array.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"

using namespace sandvik;

extern "C" {

	JNIEXPORT void JNICALL Java_com_android_i18n_timezone_internal_Memory_peekByteArray(JNIEnv* env, jclass, jlong address, jbyteArray dest, jint offset,
	                                                                                    jint length) {
		if (dest == nullptr) {
			throw NullPointerException("array is null");
		}
		if (offset < 0 || length < 0 || static_cast<size_t>(offset + length) > static_cast<size_t>(env->GetArrayLength(dest))) {
			throw IndexOutOfBoundsException("Invalid offset or length for the given array");
		}
		const jbyte* srcPtr = reinterpret_cast<const jbyte*>(static_cast<uintptr_t>(address));
		env->SetByteArrayRegion(dest, offset, length, srcPtr);
	}

}  // extern "C"