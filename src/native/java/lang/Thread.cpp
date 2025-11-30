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

#include <fmt/format.h>
#include <jni/jni.h>

#include <chrono>
#include <thread>

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

extern "C" {
	JNIEXPORT void JNICALL Java_java_lang_Thread_registerNatives(JNIEnv* env, jclass clazz) {
		// Currently no native methods need explicit registration here.
		// Keep parameters referenced to avoid unused-parameter warnings.
		(void)env;
		(void)clazz;
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Thread_currentThread(JNIEnv* env, jobject obj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto& vm = jenv->getVm();
		auto threadObj = vm.currentThread().getThreadObject();
		return (jobject)threadObj;
	}

	JNIEXPORT void JNICALL Java_java_lang_Thread_start0(JNIEnv* env, jobject obj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto& vm = jenv->getVm();
		auto& thread = vm.newThread(sandvik::native::getObject(obj));
		thread.run();
	}

	JNIEXPORT void JNICALL Java_java_lang_Thread_sleep__J(JNIEnv* env, jclass /*clazz*/, jlong millis) {
		if (millis < 0) {
			throw sandvik::IllegalArgumentException("timeout value is negative");
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long long>(millis)));
	}

	JNIEXPORT void JNICALL Java_java_lang_Thread_join__J(JNIEnv* env, jobject obj, jlong millis) {
		if (millis < 0) {
			throw sandvik::IllegalArgumentException("timeout value is negative");
		}
		auto jenv = sandvik::native::getNativeInterface(env);
		auto& vm = jenv->getVm();
		auto tobj = sandvik::native::getObject(obj);
		auto tname = tobj->getField("name")->str();
		logger.fdebug("Joining thread '{}' for {} milliseconds", tname, millis);
		auto& target = vm.getThread(tname);
		target.join();
		// @todo: timeout handling not implemented yet
	}
}
