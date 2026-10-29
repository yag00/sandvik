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

#include <cstdint>
#include <string>
#include <vector>

#include "classloader.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"

namespace {
	struct ParcelState {
			std::string interfaceToken;
			std::vector<std::string> strings;
			int position = 0;
	};

	ParcelState* state(jlong nativePtr_) {
		return reinterpret_cast<ParcelState*>(static_cast<uintptr_t>(nativePtr_));
	}
}  // namespace

extern "C" {

	JNIEXPORT jlong JNICALL Java_android_os_Parcel_nativeCreate(JNIEnv*, jclass) {
		return static_cast<jlong>(reinterpret_cast<uintptr_t>(new ParcelState()));
	}

	JNIEXPORT void JNICALL Java_android_os_Parcel_nativeDestroy(JNIEnv*, jclass, jlong nativePtr) {
		delete state(nativePtr);
	}

	JNIEXPORT void JNICALL Java_android_os_Parcel_nativeWriteInterfaceToken(JNIEnv* env, jclass, jlong nativePtr, jstring interfaceToken) {
		state(nativePtr)->interfaceToken = sandvik::native::getString(interfaceToken)->str();
	}

	JNIEXPORT void JNICALL Java_android_os_Parcel_nativeWriteString16(JNIEnv* env, jclass, jlong nativePtr, jstring value) {
		state(nativePtr)->strings.push_back(sandvik::native::getString(value)->str());
	}

	JNIEXPORT void JNICALL Java_android_os_Parcel_nativeSetDataPosition(JNIEnv*, jclass, jlong nativePtr, jint position) {
		state(nativePtr)->position = position;
	}

	JNIEXPORT jint JNICALL Java_android_os_Parcel_nativeReadInt(JNIEnv*, jclass, jlong) {
		return 0;
	}

	JNIEXPORT jobject JNICALL Java_android_os_Parcel_nativeReadStrongBinder(JNIEnv* env, jclass, jlong) {
		auto& classloader = sandvik::native::getNativeInterface(env)->getClassLoader();
		return (jobject)sandvik::Object::make(classloader.getOrLoad("android/os/Binder"));
	}

	JNIEXPORT void JNICALL Java_android_os_Parcel_nativeFreeBuffer(JNIEnv*, jclass, jlong) {
	}

	JNIEXPORT void JNICALL Java_android_os_Parcel_nativeWriteStrongBinder(JNIEnv*, jclass, jlong, jobject) {
	}

	JNIEXPORT jint JNICALL Java_android_os_Parcel_nativeWriteInt(JNIEnv*, jclass, jlong, jint) {
		return 0;
	}

	JNIEXPORT jint JNICALL Java_android_os_Parcel_nativeWriteLong(JNIEnv*, jclass, jlong, jlong) {
		return 0;
	}

}  // extern "C"
