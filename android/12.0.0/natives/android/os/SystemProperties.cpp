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
		logger.fwarning("SystemProperties.native_getString called with key: {}, returning default value", keyStr);
		return def;
	}

	// Resolves an Integer system property key with a Java-provided default fallback.
	JNIEXPORT jint JNICALL Java_android_os_SystemProperties_native_get_int__Ljava_lang_String_2I(JNIEnv* env, jclass clazz, jstring key, jint def) {
		(void)env;
		(void)clazz;

		auto keyStr = sandvik::native::getString(key)->str();
		if (sandvik::GetProp::getInstance().has(keyStr)) {
			try {
				auto value = sandvik::GetProp::getInstance().get(keyStr);
				return static_cast<jint>(std::stoi(value));
			} catch (const std::exception&) {
				logger.fwarning("SystemProperties.native_getInt invalid numeric value for key: {}", keyStr);
			}
		}
		logger.fwarning("SystemProperties.native_getInt called with key: {}, returning default value", keyStr);
		return def;
	}

	// Resolves a Boolean system property key with a Java-provided default fallback.
	JNIEXPORT jboolean JNICALL Java_android_os_SystemProperties_native_get_boolean__Ljava_lang_String_2Z(JNIEnv* env, jclass clazz, jstring key, jboolean def) {
		(void)env;
		(void)clazz;

		auto keyStr = sandvik::native::getString(key)->str();
		if (sandvik::GetProp::getInstance().has(keyStr)) {
			bool parsed = false;
			if (parseBool(sandvik::GetProp::getInstance().get(keyStr), parsed)) {
				return static_cast<jboolean>(parsed);
			}
			logger.fwarning("SystemProperties.native_getBoolean invalid bool value for key: {}", keyStr);
		}
		logger.fwarning("SystemProperties.native_getBoolean called with key: {}, returning default value", keyStr);
		return def;
	}

}  // extern "C"
