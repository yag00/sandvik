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

#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

extern "C" {

	// Resolves a String system property key with a Java-provided default fallback.
	JNIEXPORT jstring JNICALL Java_android_os_SystemProperties_native_get__Ljava_lang_String_2Ljava_lang_String_2(JNIEnv* env, jclass clazz, jstring key,
	                                                                                                              jstring def) {
		(void)env;
		(void)clazz;

		auto keyStr = sandvik::native::getString(key)->str();
		if (keyStr == "ro.product.cpu.abilist64") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("x86_64"));
		}
		if (keyStr == "ro.product.cpu.abilist32") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("x86"));
		}
		if (keyStr == "ro.product.cpu.abilist") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("x86_64,x86"));
		}
		if (keyStr == "ro.product.cpu.abi") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("x86_64"));
		}
		if (keyStr == "ro.boot.hardware.sku") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("Sandvik HW SKU"));
		}
		if (keyStr == "ro.boot.product.hardware.sku") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("Sandvik Product HW SKU"));
		}
		if (keyStr == "ro.boot.qemu") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("0"));
		}
		if (keyStr == "ro.bootloader") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("Sandvik BL"));
		}
		if (keyStr == "ro.build.date.utc") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("1735689600"));
		}
		if (keyStr == "ro.build.display.id") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("Sandvik Build ID"));
		}
		if (keyStr == "ro.build.fingerprint") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(),
			                                      std::string("Sandvik/Sandvik/Sandvik:12/SP1A.210812.016/1735689600:user/release-keys"));
		}
		if (keyStr == "ro.build.host") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("Sandvik Host"));
		}
		if (keyStr == "ro.build.id") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("SP1A.210812.016"));
		}
		if (keyStr == "ro.build.tags") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("release-keys"));
		}
		if (keyStr == "ro.build.type") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("user"));
		}
		if (keyStr == "ro.build.user") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("android-build"));
		}
		if (keyStr == "ro.build.version.release") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("12"));
		}
		if (keyStr == "ro.build.version.incremental") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string(""));
		}
		if (keyStr == "ro.hardware") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("Sandvik Hardware"));
		}
		if (keyStr == "ro.product.board") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("Sandvik Board"));
		}
		if (keyStr == "ro.product.brand") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("Sandvik Brand"));
		}
		if (keyStr == "ro.product.device") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("Sandvik Device"));
		}
		if (keyStr == "ro.product.manufacturer") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("Sandvik Manufacturer"));
		}
		if (keyStr == "ro.product.model") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("Sandvik Model"));
		}
		if (keyStr == "ro.product.name") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("Sandvik Name"));
		}
		if (keyStr == "ro.soc.manufacturer") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("Sandvik SoC Manufacturer"));
		}
		if (keyStr == "ro.soc.model") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("Sandvik SoC Model"));
		}
		if (keyStr == "sys.boot_completed") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("1"));
		}
		if (keyStr == "no.such.thing") {
			return (jstring)sandvik::Object::make(sandvik::native::getNativeInterface(env)->getClassLoader(), std::string("default_value"));
		}
		logger.fwarning("SystemProperties.native_getString called with key: {}, returning default value", keyStr);
		return def;
	}

	// Resolves an Integer system property key with a Java-provided default fallback.
	JNIEXPORT jint JNICALL Java_android_os_SystemProperties_native_get_int__Ljava_lang_String_2I(JNIEnv* env, jclass clazz, jstring key, jint def) {
		(void)env;
		(void)clazz;

		auto keyStr = sandvik::native::getString(key)->str();
		if (keyStr == "ro.hw_timeout_multiplier") {
			return (jint)4;
		}
		if (keyStr == "ro.debuggable") {
			return (jint)1;
		}
		logger.fwarning("SystemProperties.native_getInt called with key: {}, returning default value", keyStr);
		return def;
	}

	// Resolves a Boolean system property key with a Java-provided default fallback.
	JNIEXPORT jboolean JNICALL Java_android_os_SystemProperties_native_get_boolean__Ljava_lang_String_2Z(JNIEnv* env, jclass clazz, jstring key, jboolean def) {
		(void)env;
		(void)clazz;

		auto keyStr = sandvik::native::getString(key)->str();
		if (keyStr == "ro.treble.enabled") {
			return (jboolean) true;
		}
		if (keyStr == "ro.boot.container") {
			return (jboolean) false;
		}
		logger.fwarning("SystemProperties.native_getBoolean called with key: {}, returning default value", keyStr);
		return def;
	}

}  // extern "C"
