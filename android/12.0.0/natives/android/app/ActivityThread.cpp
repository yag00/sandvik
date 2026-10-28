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

// No hwui renderer nor heapprofd in Sandvik

extern "C" {

	JNIEXPORT void JNICALL Java_android_app_ActivityThread_nPurgePendingResources(JNIEnv*, jobject) {
	}

	JNIEXPORT void JNICALL Java_android_app_ActivityThread_nDumpGraphicsInfo(JNIEnv*, jobject, jobject) {
	}

	JNIEXPORT void JNICALL Java_android_app_ActivityThread_nInitZygoteChildHeapProfiling(JNIEnv*, jobject) {
	}

}  // extern "C"
