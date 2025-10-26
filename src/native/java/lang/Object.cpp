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

#include "object.hpp"

#include <fmt/format.h>
#include <jni/jni.h>

#include <chrono>
#include <thread>

#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "jnihandlemap.hpp"
#include "jthread.hpp"
#include "native/native_utils.hpp"
#include "system/logger.hpp"
#include "vm.hpp"

extern "C" {
	JNIEXPORT void JNICALL Java_java_lang_Object_registerNatives(JNIEnv* env, jclass clazz) {
		// Currently no native methods need explicit registration here.
		// Keep parameters referenced to avoid unused-parameter warnings.
		(void)env;
		(void)clazz;
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Object_getClass(JNIEnv* env, jobject obj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto ptr = sandvik::native::getObject(obj);
		if (ptr->isClass()) {
			auto clazz = sandvik::Object::make(ptr->getClass());
			jobject jclassObj = jenv->getHandles().toJObject(clazz);
			return jclassObj;
		} else {
			throw sandvik::ClassCastException("Object is not a java.lang.Class");
		}
	}

	JNIEXPORT void JNICALL Java_java_lang_Object_notify(JNIEnv* env, jobject obj) {
		auto this_ptr = sandvik::native::getObject(obj);
		this_ptr->notify();
	}
	JNIEXPORT void JNICALL Java_java_lang_Object_notifyAll(JNIEnv* env, jobject obj) {
		auto this_ptr = sandvik::native::getObject(obj);
		this_ptr->notifyAll();
	}
	JNIEXPORT void JNICALL Java_java_lang_Object_wait__J(JNIEnv* env, jobject obj, jlong timeout) {
		if (timeout < 0) {
			throw sandvik::IllegalArgumentException("timeout value is negative");
		}
		auto this_ptr = sandvik::native::getObject(obj);
		this_ptr->wait(static_cast<uint64_t>(timeout));
	}
}
