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

// No GPU driver nor Vulkan/GLES loader in Sandvik: the graphics environment settings are ignored

extern "C" {

	JNIEXPORT jboolean JNICALL Java_android_os_GraphicsEnvironment_isDebuggable(JNIEnv*, jclass) {
		return JNI_FALSE;
	}

	JNIEXPORT void JNICALL Java_android_os_GraphicsEnvironment_setLayerPaths__Ljava_lang_ClassLoader_2Ljava_lang_String_2(JNIEnv*, jclass, jobject, jstring) {
	}

	JNIEXPORT void JNICALL Java_android_os_GraphicsEnvironment_setDebugLayers(JNIEnv*, jclass, jstring) {
	}

	JNIEXPORT void JNICALL Java_android_os_GraphicsEnvironment_setDebugLayersGLES(JNIEnv*, jclass, jstring) {
	}

	JNIEXPORT void JNICALL Java_android_os_GraphicsEnvironment_setDriverPathAndSphalLibraries(JNIEnv*, jclass, jstring, jstring) {
	}

	JNIEXPORT void JNICALL Java_android_os_GraphicsEnvironment_setGpuStats(JNIEnv*, jclass, jstring, jstring, jlong, jlong, jstring, jint) {
	}

	JNIEXPORT void JNICALL Java_android_os_GraphicsEnvironment_setAngleInfo(JNIEnv*, jclass, jstring, jstring, jstring, jobjectArray) {
	}

	JNIEXPORT jboolean JNICALL Java_android_os_GraphicsEnvironment_getShouldUseAngle(JNIEnv*, jclass, jstring) {
		return JNI_FALSE;
	}

	JNIEXPORT jboolean JNICALL Java_android_os_GraphicsEnvironment_setInjectLayersPrSetDumpable(JNIEnv*, jclass) {
		return JNI_FALSE;
	}

	JNIEXPORT void JNICALL Java_android_os_GraphicsEnvironment_hintActivityLaunch(JNIEnv*, jclass) {
	}

}  // extern "C"
