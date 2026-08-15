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
#include <jni/jni.h>

#include "array.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"

using namespace sandvik;

extern "C" {

	JNIEXPORT jlong JNICALL Java_com_android_icu_charset_NativeConverter_getNativeFinalizer(JNIEnv* env, jclass) {
		// No native cleanup needed in this VM; returning 0 disables the finalizer.
		return 0;
	}

	JNIEXPORT jobject JNICALL Java_com_android_icu_charset_NativeConverter_charsetForName(JNIEnv* env, jclass, jstring charsetName) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto& classloader = jenv->getClassLoader();

		auto nameObj = sandvik::native::getString(charsetName);
		if (!nameObj) {
			return nullptr;
		}

		auto& charsetClass = classloader.getOrLoad("java/nio/charset/Charset");
		jclass jcharsetClass = (jclass)Object::make(charsetClass);

		jmethodID ctorID = env->GetMethodID(jcharsetClass, "<init>", "(Ljava/lang/String;[Ljava/lang/String;)V");
		if (!ctorID) {
			throw NoSuchMethodException("Charset constructor (Ljava/lang/String;[Ljava/lang/String;)V not found");
		}

		// Empty aliases array: no known aliases for this minimal stub.
		auto& stringClass = classloader.getOrLoad("java/lang/String");
		auto aliasesArray = Array::make(stringClass, 0u);

		return env->NewObject(jcharsetClass, ctorID, (jobject)Object::make(classloader, nameObj->str()), (jobject)aliasesArray);
	}
}