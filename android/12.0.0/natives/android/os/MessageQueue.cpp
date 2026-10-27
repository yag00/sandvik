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

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>

namespace {
	struct MessageQueueState {
			std::mutex mutex;
			std::condition_variable wake;
			bool notified = false;
	};

	MessageQueueState* state(jlong nativePtr_) {
		return reinterpret_cast<MessageQueueState*>(static_cast<uintptr_t>(nativePtr_));
	}
}  // namespace

extern "C" {

	JNIEXPORT jlong JNICALL Java_android_os_MessageQueue_nativeInit(JNIEnv*, jclass) {
		return static_cast<jlong>(reinterpret_cast<uintptr_t>(new MessageQueueState()));
	}

	JNIEXPORT void JNICALL Java_android_os_MessageQueue_nativeDestroy(JNIEnv*, jclass, jlong nativePtr) {
		delete state(nativePtr);
	}

	JNIEXPORT jboolean JNICALL Java_android_os_MessageQueue_nativeIsPolling(JNIEnv*, jclass, jlong) {
		return JNI_FALSE;
	}

	JNIEXPORT void JNICALL Java_android_os_MessageQueue_nativePollOnce(JNIEnv*, jobject, jlong nativePtr, jint timeoutMillis) {
		auto* queue = state(nativePtr);
		std::unique_lock lock(queue->mutex);
		if (timeoutMillis < 0) {
			queue->wake.wait(lock, [&queue] { return queue->notified; });
		} else if (timeoutMillis > 0) {
			queue->wake.wait_for(lock, std::chrono::milliseconds(timeoutMillis), [&queue] { return queue->notified; });
		}
		queue->notified = false;
	}

	JNIEXPORT void JNICALL Java_android_os_MessageQueue_nativeSetFileDescriptorEvents(JNIEnv*, jclass, jlong, jint, jint) {
	}

	JNIEXPORT void JNICALL Java_android_os_MessageQueue_nativeWake(JNIEnv*, jclass, jlong nativePtr) {
		auto* queue = state(nativePtr);
		{
			std::lock_guard lock(queue->mutex);
			queue->notified = true;
		}
		queue->wake.notify_one();
	}

}  // extern "C"
