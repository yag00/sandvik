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

extern "C" {

	JNIEXPORT jlong JNICALL Java_android_os_Binder_getNativeBBinderHolder(JNIEnv*, jclass) {
		return 1;
	}

	JNIEXPORT jlong JNICALL Java_android_os_Binder_getNativeFinalizer(JNIEnv*, jclass) {
		return 0;
	}

	JNIEXPORT jlong JNICALL Java_android_os_Binder_clearCallingIdentity(JNIEnv*, jclass) {
		return 0;
	}

	JNIEXPORT void JNICALL Java_android_os_Binder_restoreCallingIdentity(JNIEnv*, jclass, jlong) {
	}

	JNIEXPORT void JNICALL Java_android_os_Binder_flushPendingCommands(JNIEnv*, jclass) {
	}

}  // extern "C"
