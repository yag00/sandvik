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

#include "class.hpp"

#include <fmt/format.h>
#include <jni/jni.h>

#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "jnihandlemap.hpp"
#include "native/native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

extern "C" {
	JNIEXPORT jstring JNICALL Java_java_lang_Class_getName(JNIEnv* env, jobject obj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto ptr = sandvik::native::getObject(obj);
		std::string name = ptr->getClassType().getFullname();
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		auto strObj = sandvik::Object::make(classloader, name);
		jobject jstr = jenv->getHandles().toJObject(strObj);
		return (jstring)jstr;
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

	JNIEXPORT jobject JNICALL Java_java_lang_Class_forName(JNIEnv* env, jclass, jstring name) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto objstr = sandvik::native::getString(name);
		auto& classloader = jenv->getClassLoader();
		auto& clazz = classloader.getOrLoad(objstr->str());
		auto classObj = sandvik::Object::make(clazz);
		jobject jclassObj = jenv->getHandles().toJObject(classObj);
		return jclassObj;
	}
}