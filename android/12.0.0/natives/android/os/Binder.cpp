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

#include <unistd.h>

#include <jni/jni.h>

namespace {
	// Per-thread IPCThreadState values, there is no incoming transaction in Sandvik
	thread_local jint sStrictModePolicy = 0;
	constexpr jint UNSET_WORKSOURCE = -1;
	thread_local jint sWorkSourceUid = UNSET_WORKSOURCE;
}  // namespace

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

	// Outside a transaction the caller is the current process
	JNIEXPORT jint JNICALL Java_android_os_Binder_getCallingPid(JNIEnv*, jclass) {
		return static_cast<jint>(getpid());
	}

	JNIEXPORT jint JNICALL Java_android_os_Binder_getCallingUid(JNIEnv*, jclass) {
		return static_cast<jint>(getuid());
	}

	JNIEXPORT jboolean JNICALL Java_android_os_Binder_isHandlingTransaction(JNIEnv*, jclass) {
		return JNI_FALSE;
	}

	JNIEXPORT void JNICALL Java_android_os_Binder_setThreadStrictModePolicy(JNIEnv*, jclass, jint policy) {
		sStrictModePolicy = policy;
	}

	JNIEXPORT jint JNICALL Java_android_os_Binder_getThreadStrictModePolicy(JNIEnv*, jclass) {
		return sStrictModePolicy;
	}

	// Work source calls return a token restoring the previous work source uid
	JNIEXPORT jlong JNICALL Java_android_os_Binder_setCallingWorkSourceUid(JNIEnv*, jclass, jint uid) {
		const jlong token = sWorkSourceUid;
		sWorkSourceUid = uid;
		return token;
	}

	JNIEXPORT jint JNICALL Java_android_os_Binder_getCallingWorkSourceUid(JNIEnv*, jclass) {
		return sWorkSourceUid;
	}

	JNIEXPORT jlong JNICALL Java_android_os_Binder_clearCallingWorkSource(JNIEnv*, jclass) {
		const jlong token = sWorkSourceUid;
		sWorkSourceUid = UNSET_WORKSOURCE;
		return token;
	}

	JNIEXPORT void JNICALL Java_android_os_Binder_restoreCallingWorkSource(JNIEnv*, jclass, jlong token) {
		sWorkSourceUid = static_cast<jint>(token);
	}

	JNIEXPORT void JNICALL Java_android_os_Binder_markVintfStability(JNIEnv*, jobject) {
	}

	JNIEXPORT void JNICALL Java_android_os_Binder_forceDowngradeToSystemStability(JNIEnv*, jobject) {
	}

	JNIEXPORT void JNICALL Java_android_os_Binder_blockUntilThreadAvailable(JNIEnv*, jclass) {
	}

	JNIEXPORT jobject JNICALL Java_android_os_Binder_getExtension(JNIEnv*, jobject) {
		return nullptr;
	}

	JNIEXPORT void JNICALL Java_android_os_Binder_setExtension(JNIEnv*, jobject, jobject) {
	}

}  // extern "C"
