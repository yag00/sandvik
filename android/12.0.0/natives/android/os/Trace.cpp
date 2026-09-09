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

	JNIEXPORT jlong JNICALL Java_android_os_Trace_nativeGetEnabledTags(JNIEnv* env, jclass clazz) {
		return 0xffffffffffffffffULL;
	}

	JNIEXPORT void JNICALL Java_android_os_Trace_nativeTraceBegin(JNIEnv* env, jclass clazz, jlong tag, jstring name) {
		(void)env;
		(void)clazz;
		(void)tag;
		(void)name;
	}

	JNIEXPORT void JNICALL Java_android_os_Trace_nativeTraceEnd(JNIEnv* env, jclass clazz, jlong tag) {
		(void)env;
		(void)clazz;
		(void)tag;
	}

}  // extern "C"