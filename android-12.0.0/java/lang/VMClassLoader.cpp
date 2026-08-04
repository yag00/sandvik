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

	JNIEXPORT jobjectArray JNICALL Java_java_lang_VMClassLoader_getBootClassPathEntries(JNIEnv* env, jclass) {
		auto jenv = native::getNativeInterface(env);
		auto& classloader = jenv->getClassLoader();
		const auto& entries = classloader.getClassPathEntries();
		auto& stringClass = classloader.getOrLoad("java/lang/String");
		auto array = Array::make(stringClass, static_cast<uint32_t>(entries.size()));
		for (size_t i = 0; i < entries.size(); ++i) {
			array->setElement(static_cast<uint32_t>(i), Object::make(classloader, entries[i]));
		}
		return (jobjectArray)array;
	}

	JNIEXPORT jobject JNICALL Java_java_lang_VMClassLoader_findLoadedClass(JNIEnv* env, jclass, jobject loader, jstring name) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto& classloader = jenv->getClassLoader();

		auto nameObj = sandvik::native::getString(name);
		std::string classname = nameObj->str();

		if (!classloader.isLoaded(classname)) {
			return nullptr;
		}

		auto& cls = classloader.getOrLoad(classname);
		return (jobject)Object::makeConstClass(classloader, cls);
	}

}  // extern "C"
