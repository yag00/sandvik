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

#include <time.h>

#include <jni/jni.h>

extern "C" {

	// Returns monotonic elapsed time in milliseconds used by framework boot timing.
	JNIEXPORT jlong JNICALL Java_android_os_SystemClock_elapsedRealtime(JNIEnv* env, jclass clazz) {
		(void)env;
		(void)clazz;

		timespec ts{};
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return static_cast<jlong>(ts.tv_sec) * 1000LL + static_cast<jlong>(ts.tv_nsec / 1000000LL);
	}

	// Returns monotonic uptime in milliseconds used by framework boot timing.
	JNIEXPORT jlong JNICALL Java_android_os_SystemClock_uptimeMillis(JNIEnv* env, jclass clazz) {
		(void)env;
		(void)clazz;

		timespec ts{};
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return static_cast<jlong>(ts.tv_sec) * 1000LL + static_cast<jlong>(ts.tv_nsec / 1000000LL);
	}

}  // extern "C"
