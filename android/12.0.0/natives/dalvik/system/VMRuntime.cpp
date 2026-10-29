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
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "version.hpp"

using namespace sandvik;

extern "C" {
	JNIEXPORT jstring JNICALL Java_dalvik_system_VMRuntime_bootClassPath(JNIEnv* env, jclass) {
		std::string bootClassPath = "";
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		return (jstring)sandvik::Object::make(classloader, bootClassPath);
	}
	JNIEXPORT jstring JNICALL Java_dalvik_system_VMRuntime_classPath(JNIEnv* env, jclass) {
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		std::string classPath = classloader.getClassPath();
		if (classPath.empty()) {
			// No user class path (--apk): "." as the JVM default, the system class loader would reference an empty path
			classPath = ".";
		}
		return (jstring)sandvik::Object::make(classloader, classPath);
	}
	JNIEXPORT jstring JNICALL Java_dalvik_system_VMRuntime_vmVersion(JNIEnv* env, jclass) {
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		std::string vmVersion = fmt::format("Sandvik {} ({})", sandvik::version::getVersion(), sandvik::version::getShortCommit());
		return (jstring)sandvik::Object::make(classloader, vmVersion);
	}

	JNIEXPORT jobjectArray JNICALL Java_dalvik_system_VMRuntime_properties(JNIEnv* env, jclass) {
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		std::vector<std::string> properties = {
		    "sandvik.version=" + sandvik::version::getVersion(),
		    "sandvik.commit=" + sandvik::version::getShortCommit(),
		};
		sandvik::Class& stringClass = classloader.getOrLoad("java/lang/String");
		sandvik::Array* array = sandvik::Array::make(stringClass, properties.size());
		for (size_t i = 0; i < properties.size(); i++) {
			array->setElement(i, sandvik::Object::make(classloader, properties[i]));
		}
		return (jobjectArray)array;
	}

	// Reports runtime pointer width so framework code can select 32-bit or 64-bit behavior.
	JNIEXPORT jboolean JNICALL Java_dalvik_system_VMRuntime_is64Bit(JNIEnv* env, jobject thiz) {
		(void)env;
		(void)thiz;
		return sizeof(void*) == 8 ? JNI_TRUE : JNI_FALSE;
	}

	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_preloadDexCaches(JNIEnv* env, jclass) {
		(void)env;
		// No-op for Sandvik VM, as it does not use dex caches.
	}

	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_registerNativeAllocation__J(JNIEnv*, jobject, jlong) {
	}

	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_registerNativeFree__J(JNIEnv*, jobject, jlong) {
	}

	JNIEXPORT jint JNICALL Java_dalvik_system_VMRuntime_getNotifyNativeInterval(JNIEnv*, jclass) {
		// Same as ART kNotifyNativeInterval: allocations between two native allocation notifications
		return 384;
	}

	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_notifyNativeAllocationsInternal(JNIEnv*, jobject) {
	}

	JNIEXPORT jobject JNICALL Java_dalvik_system_VMRuntime_newUnpaddedArray(JNIEnv* env, jobject thiz, jobject componentType, jint minLen) {
		(void)thiz;

		auto classObj = sandvik::native::getReferenceObjectOrNull(componentType);
		if (classObj == nullptr || !classObj->isClass()) {
			throw sandvik::NullPointerException("componentType is null");
		}

		if (minLen < 0) {
			throw sandvik::VmException("newUnpaddedArray: negative length {}", minLen);
		}

		auto& classloader = sandvik::native::getNativeInterface(env)->getClassLoader();
		const Class& elementType = classObj->getClassType();

		sandvik::ObjectRef array = sandvik::Object::makeArray(classloader, elementType, {static_cast<uint32_t>(minLen)});

		return (jobject)array;
	}
	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_updateProcessState(JNIEnv*, jobject, jint) {
		// No-op: no process scheduler to tune
	}

	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_registerSensitiveThread(JNIEnv*, jclass) {
	}

	// Application process setup (ActivityThread.handleBindApplication): nothing to configure in Sandvik

	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_setProcessPackageName(JNIEnv*, jclass, jstring) {
	}

	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_setProcessDataDirectory(JNIEnv*, jclass, jstring) {
	}

	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_registerAppInfo(JNIEnv*, jclass, jstring, jstring, jstring, jobjectArray, jint) {
	}

	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_setTargetSdkVersionNative(JNIEnv*, jobject, jint) {
	}

	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_setDisabledCompatChangesNative(JNIEnv*, jobject, jlongArray) {
	}

	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_setHiddenApiExemptions(JNIEnv*, jobject, jobjectArray) {
	}

	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_setHiddenApiAccessLogSamplingRate(JNIEnv*, jobject, jint) {
	}

	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_setDedupeHiddenApiWarnings(JNIEnv*, jclass, jboolean) {
	}

	JNIEXPORT jboolean JNICALL Java_dalvik_system_VMRuntime_isJavaDebuggable(JNIEnv*, jobject) {
		return JNI_FALSE;
	}

	JNIEXPORT jboolean JNICALL Java_dalvik_system_VMRuntime_isNativeDebuggable(JNIEnv*, jobject) {
		return JNI_FALSE;
	}

	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_clampGrowthLimit(JNIEnv*, jobject) {
	}

	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_clearGrowthLimit(JNIEnv*, jobject) {
	}

	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_notifyStartupCompleted(JNIEnv*, jobject) {
	}

	JNIEXPORT void JNICALL Java_dalvik_system_VMRuntime_bootCompleted(JNIEnv*, jclass) {
	}

	JNIEXPORT jstring JNICALL Java_dalvik_system_VMRuntime_getCurrentInstructionSet(JNIEnv* env, jclass) {
		return env->NewStringUTF("x86_64");
	}

	JNIEXPORT jstring JNICALL Java_dalvik_system_VMRuntime_vmInstructionSet(JNIEnv* env, jobject) {
		return env->NewStringUTF("x86_64");
	}

	// Sandvik objects never move: a non movable array is a plain array
	JNIEXPORT jobject JNICALL Java_dalvik_system_VMRuntime_newNonMovableArray(JNIEnv* env, jobject thiz, jobject componentType, jint length) {
		return Java_dalvik_system_VMRuntime_newUnpaddedArray(env, thiz, componentType, length);
	}

	// Sandvik array elements are not stored as a contiguous primitive buffer: the "address" is an opaque handle on the
	// array itself, only usable by natives that write it back through JNI (TypedArray data for AssetManager natives)
	JNIEXPORT jlong JNICALL Java_dalvik_system_VMRuntime_addressOf(JNIEnv*, jobject, jobject array) {
		auto obj = sandvik::native::getReferenceObjectOrNull(array);
		if (obj == nullptr || !obj->isArray()) {
			throw sandvik::IllegalArgumentException("addressOf: not an array");
		}
		return static_cast<jlong>(reinterpret_cast<uintptr_t>(array));
	}

}  // extern "C"
