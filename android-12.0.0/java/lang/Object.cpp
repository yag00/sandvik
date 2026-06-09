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

#include "object.hpp"

#include <fmt/format.h>
#include <jni/jni.h>

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "system/logger.hpp"

using namespace sandvik;

extern "C" {

#if 0
JNIEXPORT void JNICALL Java_java_lang_Object_registerNatives(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Object_hashCode(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Object_identityHashCodeNative(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

	JNIEXPORT jobject JNICALL Java_java_lang_Object_internalClone(JNIEnv* env, jobject obj) {
		auto this_ptr = sandvik::native::getObject(obj);
		auto cloned_obj = this_ptr->clone();
		return (jobject)cloned_obj;
	}

	JNIEXPORT void JNICALL Java_java_lang_Object_notify(JNIEnv* env, jobject obj) {
		auto this_ptr = sandvik::native::getObject(obj);
		this_ptr->notify();
	}
	JNIEXPORT void JNICALL Java_java_lang_Object_notifyAll(JNIEnv* env, jobject obj) {
		auto this_ptr = sandvik::native::getObject(obj);
		this_ptr->notifyAll();
	}

	JNIEXPORT void JNICALL Java_java_lang_Object_wait__J(JNIEnv* env, jobject obj, jlong millis) {
		if (millis < 0) {
			throw IllegalArgumentException("timeout value is negative");
		}
		auto this_ptr = sandvik::native::getObject(obj);
		this_ptr->wait(static_cast<uint64_t>(millis));
	}

	JNIEXPORT void JNICALL Java_java_lang_Object_wait__JI(JNIEnv* env, jobject obj, jlong millis, jint nanos) {
		if (millis < 0) {
			throw IllegalArgumentException("timeout value is negative");
		}
		if (nanos < 0 || nanos > 999999) {
			throw IllegalArgumentException("nanosecond timeout value out of range");
		}

		uint64_t timeoutMs = static_cast<uint64_t>(millis);
		if (nanos >= 500000 || (nanos != 0 && timeoutMs == 0)) {
			timeoutMs++;
		}

		auto this_ptr = sandvik::native::getObject(obj);
		this_ptr->wait(timeoutMs);
	}

}  // extern "C"
