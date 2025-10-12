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
		auto& objclass = classloader.getOrLoad(objstr->str());
		logger.debug(fmt::format("Class.forName: Loaded class {}", objclass.getFullname()));
		auto& clazz = classloader.getOrLoad("java/lang/Class");
		auto classObj = sandvik::Object::make(clazz);
		classObj->setField("internal", sandvik::Object::make(objclass));
		jobject jclassObj = jenv->getHandles().toJObject(classObj);
		return jclassObj;
	}
	JNIEXPORT jobject JNICALL Java_java_lang_Class_getDeclaredConstructor(JNIEnv* env, jobject obj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto classObj = sandvik::native::getObject(obj);
		auto& classloader = jenv->getClassLoader();

		auto internalClass = classObj->getField("internal");
		logger.debug(fmt::format("Class.getDeclaredConstructor: Getting constructor for class {}", internalClass->debug()));

		// Create a java.lang.reflect.Constructor object
		auto& ctorClass = classloader.getOrLoad("java/lang/reflect/Constructor");
		auto ctorObj = sandvik::Object::make(ctorClass);

		// Set fields as needed (e.g., 'declaringClass', 'parameterTypes', etc.)
		ctorObj->setField("declaringClass", sandvik::Object::make(internalClass->getClass()));
		// For simplicity, parameterTypes is empty for default constructor
		ctorObj->setField("parameterTypes", sandvik::Object::make(classloader.getOrLoad("java/lang/Object")));

		jobject jctorObj = jenv->getHandles().toJObject(ctorObj);
		return jctorObj;
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_getField(JNIEnv* env, jobject obj, jstring name) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto ptr = sandvik::native::getObject(obj);
		auto fieldNameObj = sandvik::native::getString(name);
		std::string fieldName = fieldNameObj->str();

		auto& classloader = jenv->getClassLoader();
		logger.debug(fmt::format("Class.getField: Getting field '{}' for class {}", fieldName, ptr->debug()));

		try {
			auto field = ptr->getField(fieldName);
		} catch (const std::exception& e) {
			logger.debug(fmt::format("{} Class.getField: Created missing field '{}' in class {}", e.what(), fieldName, ptr->debug()));
			ptr->setField(fieldName, sandvik::Object::make(0));
			// throw sandvik::NoSuchFieldException(fmt::format("Field '{}' not found in class {}", fieldName, ptr->getClassType().getFullname()));
		}

		// Create a java.lang.reflect.Field object
		auto& fieldClass = classloader.getOrLoad("java/lang/reflect/Field");
		auto fieldObj = sandvik::Object::make(fieldClass);

		// Set fields as needed (e.g., 'declaringClass', 'name', etc.)
		// fieldObj->setField("clazz", sandvik::Object::make(ptr));
		fieldObj->setField("name", sandvik::Object::make(classloader, fieldName));
		// Optionally set other fields like 'type', 'modifiers', etc.

		jobject jfieldObj = jenv->getHandles().toJObject(fieldObj);
		return jfieldObj;
	}
}