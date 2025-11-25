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

#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "native/native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "vm.hpp"

extern "C" {
	JNIEXPORT void JNICALL Java_java_util_concurrent_atomic_AtomicInteger_lazySet(JNIEnv* env, jobject obj, jint newValue) {
		auto* this_ptr = sandvik::native::getObject(obj);
		auto value = this_ptr->getField("value");
		value->setField("value", sandvik::Object::make(static_cast<uint64_t>(static_cast<int64_t>(newValue))));
	}

	JNIEXPORT jint JNICALL Java_java_util_concurrent_atomic_AtomicInteger_getAndSet(JNIEnv* env, jobject obj, jint newValue) {
		auto* this_ptr = sandvik::native::getObject(obj);
		auto value = this_ptr->getField("value");
		return value->getAndSet(newValue);
	}
	JNIEXPORT jboolean JNICALL Java_java_util_concurrent_atomic_AtomicInteger_compareAndSet(JNIEnv* env, jobject obj, jint expect, jint update) {
		auto* this_ptr = sandvik::native::getObject(obj);
		auto value = this_ptr->getField("value");
		return static_cast<jboolean>(value->compareAndSet(expect, update));
	}

	JNIEXPORT jboolean JNICALL Java_java_util_concurrent_atomic_AtomicInteger_weakCompareAndSet(JNIEnv* env, jobject obj, jint expect, jint update) {
		auto* this_ptr = sandvik::native::getObject(obj);
		auto value = this_ptr->getField("value");
		return static_cast<jboolean>(value->weakCompareAndSet(expect, update));
	}
	JNIEXPORT jint JNICALL Java_java_util_concurrent_atomic_AtomicInteger_getAndIncrement(JNIEnv* env, jobject obj) {
		auto* this_ptr = sandvik::native::getObject(obj);
		auto value = this_ptr->getField("value");
		return value->getAndAdd((int32_t)1);
	}
	JNIEXPORT jint JNICALL Java_java_util_concurrent_atomic_AtomicInteger_getAndDecrement(JNIEnv* env, jobject obj) {
		auto* this_ptr = sandvik::native::getObject(obj);
		auto value = this_ptr->getField("value");
		return value->getAndAdd((int32_t)-1);
	}
	JNIEXPORT jint JNICALL Java_java_util_concurrent_atomic_AtomicInteger_getAndAdd(JNIEnv* env, jobject obj, jint delta) {
		auto* this_ptr = sandvik::native::getObject(obj);
		auto value = this_ptr->getField("value");
		return value->getAndAdd(delta);
	}
	JNIEXPORT jint JNICALL Java_java_util_concurrent_atomic_AtomicInteger_incrementAndGet(JNIEnv* env, jobject obj) {
		auto* this_ptr = sandvik::native::getObject(obj);
		auto value = this_ptr->getField("value");
		return value->addAndGet((int32_t)1);
	}
	JNIEXPORT jint JNICALL Java_java_util_concurrent_atomic_AtomicInteger_decrementAndGet(JNIEnv* env, jobject obj) {
		auto* this_ptr = sandvik::native::getObject(obj);
		auto value = this_ptr->getField("value");
		return value->addAndGet((int32_t)-1);
	}
	JNIEXPORT jint JNICALL Java_java_util_concurrent_atomic_AtomicInteger_addAndGet(JNIEnv* env, jobject obj, jint delta) {
		auto* this_ptr = sandvik::native::getObject(obj);
		auto value = this_ptr->getField("value");
		return value->addAndGet(static_cast<int32_t>(delta));
	}
}