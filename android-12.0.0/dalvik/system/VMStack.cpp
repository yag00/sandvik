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

extern "C" {

	/**
	 * Native implementation of:
	 * public static native ClassLoader getCallingClassLoader();
	 *
	 * Retrieves the class loader of the caller's caller.
	 */
	JNIEXPORT jobject JNICALL Java_dalvik_system_VMStack_getCallingClassLoader(JNIEnv* env, jclass) {
		/*try {
		    // Retrieve the calling class from the stack
		    auto callingClass = sandvik::native::getCallingClass(2);  // Caller of the caller
		    if (!callingClass) {
		        return nullptr;  // No calling class found
		    }

		    // Get the class loader of the calling class
		    auto classLoader = callingClass->getClassLoader();
		    return sandvik::native::wrapObject(classLoader);
		} catch (const sandvik::VmException& e) {
		    sandvik::native::throwJavaException(env, "java.lang.RuntimeException", e.what());
		    return nullptr;
		}*/
		logger.fwarning("{} not implemented!", __FUNCTION__);
		return nullptr;
	}

	/**
	 * Native implementation of:
	 * public static native Class<?> getStackClass2();
	 *
	 * Retrieves the class of the caller's caller's caller.
	 */
	JNIEXPORT jobject JNICALL Java_dalvik_system_VMStack_getStackClass2(JNIEnv* env, jclass) {
		auto jenv = native::getNativeInterface(env);
		auto& classloader = jenv->getClassLoader();
		auto& thread = jenv->getVm().currentThread();
		auto& clazz = thread.getStackClass(2);
		return (jobject)Object::makeConstClass(classloader, clazz);
	}

	/**
	 * Native implementation of:
	 * public static native ClassLoader getClosestUserClassLoader();
	 *
	 * Retrieves the first ClassLoader on the call stack that isn't the bootstrap class loader.
	 */
	JNIEXPORT jobject JNICALL Java_dalvik_system_VMStack_getClosestUserClassLoader(JNIEnv* env, jclass) {
		logger.fwarning("{} not implemented!", __FUNCTION__);
		return nullptr;
	}

	/**
	 * Native implementation of:
	 * public static native StackTraceElement[] getThreadStackTrace(Thread t);
	 *
	 * Retrieves the stack trace of the specified thread.
	 */
	JNIEXPORT jobjectArray JNICALL Java_dalvik_system_VMStack_getThreadStackTrace(JNIEnv* env, jclass, jobject threadObj) {
		logger.fwarning("{} not implemented!", __FUNCTION__);
		return nullptr;
	}

	/**
	 * Native implementation of:
	 * public static native AnnotatedStackTraceElement[] getAnnotatedThreadStackTrace(Thread t);
	 *
	 * Retrieves an annotated stack trace from the specified thread.
	 */
	JNIEXPORT jobjectArray JNICALL Java_dalvik_system_VMStack_getAnnotatedThreadStackTrace(JNIEnv* env, jclass, jobject threadObj) {
		logger.fwarning("{} not implemented!", __FUNCTION__);
		return nullptr;
	}

	/**
	 * Native implementation of:
	 * public static native int fillStackTraceElements(Thread t, StackTraceElement[] stackTraceElements);
	 *
	 * Fills the provided array with stack trace elements for the specified thread.
	 */
	JNIEXPORT jint JNICALL Java_dalvik_system_VMStack_fillStackTraceElements(JNIEnv* env, jclass, jobject threadObj, jobjectArray elements) {
		// auto jenv = native::getNativeInterface(env);
		// auto& classloader = jenv->getClassLoader();

		auto threadRef = native::getObject(threadObj);
		logger.fwarning("Filling stack trace elements for thread object: {}", threadRef->toString());
		auto stackTraceElements = native::getArray(elements);
		logger.fwarning("Provided stack trace elements array: {}", stackTraceElements->toString());

		logger.fwarning("{} not implemented!", __FUNCTION__);
		return 0;
	}

}  // extern "C"
