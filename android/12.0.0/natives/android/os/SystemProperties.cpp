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

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "jni.hpp"
#include "loader/getprop.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

extern "C" {

	static std::string toLower(std::string value_) {
		std::transform(value_.begin(), value_.end(), value_.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		return value_;
	}

	static bool parseBool(const std::string& value_, bool& out_) {
		auto v = toLower(value_);
		if (v == "1" || v == "true" || v == "y" || v == "yes" || v == "on") {
			out_ = true;
			return true;
		}
		if (v == "0" || v == "false" || v == "n" || v == "no" || v == "off") {
			out_ = false;
			return true;
		}
		return false;
	}

	static jint getIntValue(const std::string& key_, jint def_) {
		if (sandvik::GetProp::getInstance().has(key_)) {
			try {
				return static_cast<jint>(std::stoi(sandvik::GetProp::getInstance().get(key_)));
			} catch (const std::exception&) {
				logger.fwarning("SystemProperties.native_getInt invalid numeric value for key: {}", key_);
			}
		}
		return def_;
	}

	static jlong getLongValue(const std::string& key_, jlong def_) {
		if (sandvik::GetProp::getInstance().has(key_)) {
			try {
				return static_cast<jlong>(std::stoll(sandvik::GetProp::getInstance().get(key_)));
			} catch (const std::exception&) {
				logger.fwarning("SystemProperties.native_getLong invalid numeric value for key: {}", key_);
			}
		}
		return def_;
	}

	static jboolean getBooleanValue(const std::string& key_, jboolean def_) {
		if (sandvik::GetProp::getInstance().has(key_)) {
			bool parsed = false;
			if (parseBool(sandvik::GetProp::getInstance().get(key_), parsed)) {
				return static_cast<jboolean>(parsed);
			}
			logger.fwarning("SystemProperties.native_getBoolean invalid bool value for key: {}", key_);
		}
		return def_;
	}

	// Property handles (bionic prop_info*): a stable pointer to the interned key of an existing property
	static const std::string& handleKey(jlong handle_) {
		return *reinterpret_cast<const std::string*>(static_cast<uintptr_t>(handle_));
	}

	// Resolves a String system property key with a Java-provided default fallback.
	JNIEXPORT jstring JNICALL Java_android_os_SystemProperties_native_get__Ljava_lang_String_2Ljava_lang_String_2(JNIEnv* env, jclass clazz, jstring key,
	                                                                                                              jstring def) {
		(void)env;
		(void)clazz;

		auto keyStr = sandvik::native::getString(key)->str();
		if (sandvik::GetProp::getInstance().has(keyStr)) {
			auto value = sandvik::GetProp::getInstance().get(keyStr);
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), value);
		}
		logger.fdebug("SystemProperties.native_getString called with key: {}, returning default value", keyStr);
		return def;
	}

	// Resolves an Integer system property key with a Java-provided default fallback.
	JNIEXPORT jint JNICALL Java_android_os_SystemProperties_native_get_int__Ljava_lang_String_2I(JNIEnv* env, jclass clazz, jstring key, jint def) {
		(void)env;
		(void)clazz;
		return getIntValue(sandvik::native::getString(key)->str(), def);
	}

	// Resolves a Long system property key with a Java-provided default fallback.
	JNIEXPORT jlong JNICALL Java_android_os_SystemProperties_native_get_long__Ljava_lang_String_2J(JNIEnv* env, jclass clazz, jstring key, jlong def) {
		(void)env;
		(void)clazz;
		return getLongValue(sandvik::native::getString(key)->str(), def);
	}

	// Resolves a Boolean system property key with a Java-provided default fallback.
	JNIEXPORT jboolean JNICALL Java_android_os_SystemProperties_native_get_boolean__Ljava_lang_String_2Z(JNIEnv* env, jclass clazz, jstring key, jboolean def) {
		(void)env;
		(void)clazz;
		return getBooleanValue(sandvik::native::getString(key)->str(), def);
	}

	// Returns a handle on an existing property, 0 if the property is not defined
	JNIEXPORT jlong JNICALL Java_android_os_SystemProperties_native_find(JNIEnv* env, jclass clazz, jstring key) {
		(void)env;
		(void)clazz;
		static std::mutex mutex;
		static std::unordered_map<std::string, std::unique_ptr<std::string>> handles;

		auto keyStr = sandvik::native::getString(key)->str();
		if (!sandvik::GetProp::getInstance().has(keyStr)) {
			return 0;
		}
		std::lock_guard lock(mutex);
		auto& interned = handles[keyStr];
		if (!interned) {
			interned = std::make_unique<std::string>(keyStr);
		}
		return static_cast<jlong>(reinterpret_cast<uintptr_t>(interned.get()));
	}

	JNIEXPORT jstring JNICALL Java_android_os_SystemProperties_native_get__J(JNIEnv* env, jclass clazz, jlong handle) {
		(void)clazz;
		auto value = sandvik::GetProp::getInstance().get(handleKey(handle));
		return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), value);
	}

	JNIEXPORT jint JNICALL Java_android_os_SystemProperties_native_get_int__JI(JNIEnv* env, jclass clazz, jlong handle, jint def) {
		(void)env;
		(void)clazz;
		return getIntValue(handleKey(handle), def);
	}

	JNIEXPORT jlong JNICALL Java_android_os_SystemProperties_native_get_long__JJ(JNIEnv* env, jclass clazz, jlong handle, jlong def) {
		(void)env;
		(void)clazz;
		return getLongValue(handleKey(handle), def);
	}

	JNIEXPORT jboolean JNICALL Java_android_os_SystemProperties_native_get_boolean__JZ(JNIEnv* env, jclass clazz, jlong handle, jboolean def) {
		(void)env;
		(void)clazz;
		return getBooleanValue(handleKey(handle), def);
	}

}  // extern "C"
