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
#include <sys/eventfd.h>

#include "array.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

extern "C" {
	JNIEXPORT void JNICALL Java_com_android_internal_os_Zygote_nativeInitNativeState(JNIEnv* env, jclass clazz, jint uid) {
		(void)env;
		(void)clazz;

		// AOSP reinitializes various native subsystems here after zygote fork
		// (GC state, native threading, sandboxing, etc.). Sandvik doesn't fork
		// child processes the same way and has no equivalent native subsystems
		// to reset, so this is a no-op.
		logger.fdebug("Zygote.nativeInitNativeState stub called (uid={})", uid);
	}

	JNIEXPORT jint JNICALL Java_com_android_internal_os_Zygote_nativeGetUsapPoolEventFD(JNIEnv* env, jclass clazz) {
		(void)env;
		(void)clazz;

		// Sandvik has no real USAP process pool. We hand back a real eventfd
		// so callers that poll()/select() on it behave normally (they'll just
		// never see it become readable, since nothing ever writes to it) --
		// this avoids handing out an invalid fd that could cause undefined
		// behavior in poll/epoll machinery.
		static int usapEventFd = -1;
		if (usapEventFd < 0) {
			usapEventFd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
			if (usapEventFd < 0) {
				throw VmException("nativeGetUsapPoolEventFD: eventfd() failed: {}", strerror(errno));
			}
		}
		return usapEventFd;
	}
}  // extern "C"