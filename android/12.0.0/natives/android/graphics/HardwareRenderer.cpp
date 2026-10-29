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

// No hwui render thread in Sandvik: process wide renderer settings are ignored

extern "C" {

	JNIEXPORT void JNICALL Java_android_graphics_HardwareRenderer_disableVsync(JNIEnv*, jclass) {
	}

	JNIEXPORT void JNICALL Java_android_graphics_HardwareRenderer_preload(JNIEnv*, jclass) {
	}

	JNIEXPORT jboolean JNICALL Java_android_graphics_HardwareRenderer_isWebViewOverlaysEnabled(JNIEnv*, jclass) {
		return JNI_FALSE;
	}

	JNIEXPORT void JNICALL Java_android_graphics_HardwareRenderer_setupShadersDiskCache(JNIEnv*, jclass, jstring, jstring) {
	}

	JNIEXPORT void JNICALL Java_android_graphics_HardwareRenderer_nRotateProcessStatsBuffer(JNIEnv*, jclass) {
	}

	JNIEXPORT void JNICALL Java_android_graphics_HardwareRenderer_nSetProcessStatsBuffer(JNIEnv*, jclass, jint) {
	}

	JNIEXPORT void JNICALL Java_android_graphics_HardwareRenderer_nSetIsHighEndGfx(JNIEnv*, jclass, jboolean) {
	}

	JNIEXPORT void JNICALL Java_android_graphics_HardwareRenderer_nTrimMemory(JNIEnv*, jclass, jint) {
	}

	JNIEXPORT void JNICALL Java_android_graphics_HardwareRenderer_nOverrideProperty(JNIEnv*, jclass, jstring, jstring) {
	}

	JNIEXPORT jint JNICALL Java_android_graphics_HardwareRenderer_nCopySurfaceInto(JNIEnv*, jclass, jobject, jint, jint, jint, jint, jlong) {
		// PixelCopy.ERROR_UNKNOWN
		return 1;
	}

	JNIEXPORT void JNICALL Java_android_graphics_HardwareRenderer_nSetHighContrastText(JNIEnv*, jclass, jboolean) {
	}

	JNIEXPORT void JNICALL Java_android_graphics_HardwareRenderer_nHackySetRTAnimationsEnabled(JNIEnv*, jclass, jboolean) {
	}

	JNIEXPORT void JNICALL Java_android_graphics_HardwareRenderer_nSetDebuggingEnabled(JNIEnv*, jclass, jboolean) {
	}

	JNIEXPORT void JNICALL Java_android_graphics_HardwareRenderer_nSetIsolatedProcess(JNIEnv*, jclass, jboolean) {
	}

	JNIEXPORT void JNICALL Java_android_graphics_HardwareRenderer_nSetContextPriority(JNIEnv*, jclass, jint) {
	}

	JNIEXPORT void JNICALL Java_android_graphics_HardwareRenderer_nSetDisplayDensityDpi(JNIEnv*, jclass, jint) {
	}

	JNIEXPORT void JNICALL Java_android_graphics_HardwareRenderer_nInitDisplayInfo(JNIEnv*, jclass, jint, jint, jfloat, jint, jlong, jlong) {
	}

}  // extern "C"
