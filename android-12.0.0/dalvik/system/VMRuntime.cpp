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
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "version.hpp"

using namespace sandvik;

extern "C" {
	JNIEXPORT jstring JNICALL Java_dalvik_system_VMRuntime_bootClassPath(JNIEnv* env, jclass) {
		std::string bootClassPath = "";
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		return (jstring)sandvik::Object::make(classloader, bootClassPath);
	}
	JNIEXPORT jstring JNICALL Java_dalvik_system_VMRuntime_classPath(JNIEnv* env, jclass) {
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		std::string classPath = classloader.getClassPath();
		return (jstring)sandvik::Object::make(classloader, classPath);
	}
	JNIEXPORT jstring JNICALL Java_dalvik_system_VMRuntime_vmVersion(JNIEnv* env, jclass) {
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		std::string vmVersion = fmt::format("Sandvik {} ({})", sandvik::version::getVersion(), sandvik::version::getShortCommit());
		return (jstring)sandvik::Object::make(classloader, vmVersion);
	}

	JNIEXPORT jobjectArray JNICALL Java_dalvik_system_VMRuntime_properties(JNIEnv* env, jclass) {
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		std::vector<std::string> properties = {
		    "sandvik.version=" + sandvik::version::getVersion(),
		    "sandvik.commit=" + sandvik::version::getShortCommit(),
		};
		sandvik::Class& stringClass = classloader.getOrLoad("java/lang/String");
		sandvik::Array* array = sandvik::Array::make(stringClass, properties.size());
		for (size_t i = 0; i < properties.size(); i++) {
			array->setElement(i, sandvik::Object::make(classloader, properties[i]));
		}
		return (jobjectArray)array;
	}
}  // extern "C"
