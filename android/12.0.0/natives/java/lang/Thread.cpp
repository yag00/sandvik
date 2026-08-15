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

#include <fmt/format.h>
#include <jni/jni.h>

#include <chrono>
#include <thread>

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "jthread.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "vm.hpp"

using namespace sandvik;

static constexpr int THREAD_STATUS_NEW = 0;
static constexpr int THREAD_STATUS_RUNNABLE = 1;
// static constexpr int THREAD_STATUS_BLOCKED = 2;
static constexpr int THREAD_STATUS_WAITING = 3;
// static constexpr int THREAD_STATUS_TIMED_WAITING = 4;
static constexpr int THREAD_STATUS_TERMINATED = 5;

extern "C" {

#if 0
JNIEXPORT void JNICALL Java_java_lang_Thread_registerNatives(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

	JNIEXPORT jobject JNICALL Java_java_lang_Thread_currentThread(JNIEnv* env, jclass clazz) {
		auto jenv = sandvik::native::getNativeInterface(env);
		return (jobject)jenv->getVm().currentThread().getThreadObject();
	}

#if 0
JNIEXPORT void JNICALL Java_java_lang_Thread_yield(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

	JNIEXPORT void JNICALL Java_java_lang_Thread_nativeCreate(JNIEnv* env, jclass clazz, jobject thread, jlong stacksize, jboolean daemon) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto threadObj = sandvik::native::getObject(thread);
		auto& vm = jenv->getVm();
		threadObj->setField("threadStatus", Object::make((uint64_t)THREAD_STATUS_RUNNABLE));
		threadObj->setField("eetop", Object::make((uint64_t)1));
		threadObj->setField("nativePeer", Object::make((uint64_t)1));
		threadObj->setField("nativeThread", Object::make((uint64_t)1));
		threadObj->setField("nativeTid", Object::make((uint64_t)1));
		auto& newThread = vm.newThread(threadObj);
		logger.ferror("Created thread: {} stacksize: {} daemon:{}", threadObj->getField("name")->str(), stacksize, daemon);
		newThread.run();
	}

#if 0
JNIEXPORT void JNICALL Java_java_lang_Thread_start0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Thread_interrupted(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Thread_isInterrupted(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Thread_isInterrupted(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Thread_countStackFrames(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Thread_holdsLock(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Thread_setPriority0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Thread_stop0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Thread_suspend0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Thread_resume0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Thread_interrupt0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Thread_setNativeName(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

	JNIEXPORT jint JNICALL Java_java_lang_Thread_nativeGetStatus(JNIEnv* env, jobject obj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto threadObj = sandvik::native::getObject(obj);
		auto& vm = jenv->getVm();
		auto& thread = vm.getThread(threadObj->getField("name")->str());
		auto status = thread.getState();
		logger.ferror("Thread {} status: {}", thread.getName(), static_cast<int>(status));
		switch (status) {
			case sandvik::Thread::ThreadState::NotStarted:
				return static_cast<jint>(THREAD_STATUS_NEW);
			case sandvik::Thread::ThreadState::Running:
				return static_cast<jint>(THREAD_STATUS_RUNNABLE);
			case sandvik::Thread::ThreadState::SuspendedRequested:
				return static_cast<jint>(THREAD_STATUS_WAITING);
			case sandvik::Thread::ThreadState::Suspended:
				return static_cast<jint>(THREAD_STATUS_WAITING);
			case sandvik::Thread::ThreadState::Stopped:
				return static_cast<jint>(THREAD_STATUS_TERMINATED);
			default:
				return static_cast<jint>(-1);
		}
	}

	JNIEXPORT void JNICALL Java_java_lang_Thread_sleep__Ljava_lang_Object_2JI(JNIEnv* env, jclass clazz, jobject lock, jlong millis, jint nanos) {
		if (millis < 0 || nanos < 0 || nanos > 999999) {
			throw IllegalArgumentException("timeout value is negative or nanos out of range");
		}
		auto duration = std::chrono::milliseconds(millis) + std::chrono::nanoseconds(nanos);
		if (duration.count() > 0) {
			std::this_thread::sleep_for(duration);
		}
	}

}  // extern "C"
