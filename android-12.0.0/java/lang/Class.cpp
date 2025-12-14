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
#include "class.hpp"

#include <fmt/format.h>
#include <jni/jni.h>

#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "method.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

extern "C" {

	JNIEXPORT jobject JNICALL Java_java_lang_Class_classForName(JNIEnv* env, jclass, jstring className, jboolean shouldInitialize, jobject classLoader) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto objstr = sandvik::native::getString(className);
		auto& classloader = jenv->getClassLoader();
		auto& objclass = classloader.getOrLoad(objstr->str());
		logger.debug(fmt::format("Class.forName: Loaded class {}", objclass.getFullname()));
		auto& clazz = classloader.getOrLoad("java/lang/Class");
		auto classObj = sandvik::Object::make(clazz);
		classObj->setField("internal", sandvik::Object::make(objclass));
		return (jobject)classObj;
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_newInstance(JNIEnv* env, jobject obj) {
		throw VmException("Java_java_lang_Class_newInstance not implemented!");
	}

	JNIEXPORT jstring JNICALL Java_java_lang_Class_getNameNative(JNIEnv* env, jobject obj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto ptr = sandvik::native::getObject(obj);
		std::string name = ptr->getClassType().getFullname();
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		auto strObj = sandvik::Object::make(classloader, name);
		return (jstring)strObj;
	}

	JNIEXPORT jobjectArray JNICALL Java_java_lang_Class_getInterfacesInternal(JNIEnv* env, jobject obj) {
		throw VmException("Java_java_lang_Class_getInterfacesInternal not implemented!");
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_getEnclosingMethodNative(JNIEnv* env, jobject obj) {
		throw VmException("Java_java_lang_Class_getEnclosingMethodNative not implemented!");
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_getEnclosingConstructorNative(JNIEnv* env, jobject obj) {
		throw VmException("Java_java_lang_Class_getEnclosingConstructorNative not implemented!");
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_getDeclaringClass(JNIEnv* env, jobject obj) {
		throw VmException("Java_java_lang_Class_getDeclaringClass not implemented!");
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_getEnclosingClass(JNIEnv* env, jobject obj) {
		throw VmException("Java_java_lang_Class_getEnclosingClass not implemented!");
	}

	JNIEXPORT jboolean JNICALL Java_java_lang_Class_isAnonymousClass(JNIEnv* env, jobject obj) {
		throw VmException("Java_java_lang_Class_isAnonymousClass not implemented!");
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_getPublicFieldRecursive(JNIEnv* env, jobject obj, jstring name) {
		throw VmException("Java_java_lang_Class_getPublicFieldRecursive not implemented!");
	}

	JNIEXPORT jobjectArray JNICALL Java_java_lang_Class_getDeclaredClasses(JNIEnv* env, jobject obj) {
		throw VmException("Java_java_lang_Class_getDeclaredClasses not implemented!");
	}

	JNIEXPORT jobjectArray JNICALL Java_java_lang_Class_getDeclaredFields(JNIEnv* env, jobject obj) {
		throw VmException("Java_java_lang_Class_getDeclaredFields not implemented!");
	}

	JNIEXPORT jobjectArray JNICALL Java_java_lang_Class_getDeclaredFieldsUnchecked(JNIEnv* env, jobject obj, jboolean publicOnly) {
		throw VmException("Java_java_lang_Class_getDeclaredFieldsUnchecked not implemented!");
	}

	JNIEXPORT jobjectArray JNICALL Java_java_lang_Class_getDeclaredMethodsUnchecked(JNIEnv* env, jobject obj, jboolean publicOnly) {
		throw VmException("Java_java_lang_Class_getDeclaredMethodsUnchecked not implemented!");
	}

	JNIEXPORT jobjectArray JNICALL Java_java_lang_Class_getDeclaredConstructorsInternal(JNIEnv* env, jobject obj, jboolean publicOnly) {
		throw VmException("Java_java_lang_Class_getDeclaredConstructorsInternal not implemented!");
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_getDeclaredField(JNIEnv* env, jobject obj, jstring name) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto& classloader = jenv->getClassLoader();
		auto classObj = sandvik::native::getObject(obj);
		auto fieldName = sandvik::native::getString(name);
		// Get the field from the internal class representation
		auto& clazz = classObj->getClassType();
		auto& field = clazz.getField(fieldName->str());
		// Create a java.lang.reflect.Field object
		auto& fieldClass = classloader.getOrLoad("java/lang/reflect/Field");
		auto fieldObj = sandvik::Object::make(fieldClass);
		// Set the necessary fields
		fieldObj->setField("declaringClass", Object::makeConstClass(classloader, const_cast<Class&>(clazz)));
		fieldObj->setField("name", fieldName);
		fieldObj->setField("type", Object::makeConstClass(classloader, field.getClass()));
		fieldObj->setField("accessFlags", Object::make((uint64_t)field.getAccessFlags()));
		fieldObj->setField("offset", Object::make((uint64_t)field.getIndex()));
		return (jobject)fieldObj;
	}

	JNIEXPORT jobjectArray JNICALL Java_java_lang_Class_getPublicDeclaredFields(JNIEnv* env, jobject obj) {
		throw VmException("Java_java_lang_Class_getPublicDeclaredFields not implemented!");
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_getPrimitiveClass(JNIEnv* env, jclass, jstring name) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto nameObj = sandvik::native::getString(name);
		std::string primName = nameObj->str();

		auto& classloader = jenv->getClassLoader();
		logger.debug(fmt::format("Class.getPrimitiveClass: looking up primitive '{}'", primName));

		// Obtain the internal class representation and wrap it in a java.lang.Class instance.
		auto& primClass = classloader.getOrLoad(primName);
		auto classObj = sandvik::Object::makeConstClass(classloader, primClass);
		return (jobject)classObj;
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_getDeclaredConstructorInternal(JNIEnv* env, jobject obj, jobjectArray args) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto classObj = sandvik::native::getObject(obj);
		auto& classloader = jenv->getClassLoader();

		auto internalClass = classObj->getField("internal");
		logger.debug(fmt::format("Class.getDeclaredConstructor: Getting constructor for class {}", internalClass->toString()));

		// Create a java.lang.reflect.Constructor object
		auto& ctorClass = classloader.getOrLoad("java/lang/reflect/Constructor");
		auto ctorObj = sandvik::Object::make(ctorClass);

		// Set fields as needed (e.g., 'declaringClass', 'parameterTypes', etc.)
		ctorObj->setField("declaringClass", sandvik::Object::make(internalClass->getClass()));
		// For simplicity, parameterTypes is empty for default constructor
		ctorObj->setField("parameterTypes", sandvik::Object::make(classloader.getOrLoad("java/lang/Object")));

		return (jobject)ctorObj;
	}

	JNIEXPORT jstring JNICALL Java_java_lang_Class_getInnerClassName(JNIEnv* env, jobject obj) {
		throw VmException("Java_java_lang_Class_getInnerClassName not implemented!");
	}

	JNIEXPORT jint JNICALL Java_java_lang_Class_getInnerClassFlags(JNIEnv* env, jobject obj, jint defaultValue) {
		throw VmException("Java_java_lang_Class_getInnerClassFlags not implemented!");
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_getDeclaredAnnotation(JNIEnv* env, jobject obj, jobject annotationClass) {
		throw VmException("Java_java_lang_Class_getDeclaredAnnotation not implemented!");
	}

	JNIEXPORT jobjectArray JNICALL Java_java_lang_Class_getDeclaredAnnotations(JNIEnv* env, jobject obj) {
		throw VmException("Java_java_lang_Class_getDeclaredAnnotations not implemented!");
	}

	JNIEXPORT jboolean JNICALL Java_java_lang_Class_isDeclaredAnnotationPresent(JNIEnv* env, jobject obj, jobject annotationClass) {
		throw VmException("Java_java_lang_Class_isDeclaredAnnotationPresent not implemented!");
	}

	JNIEXPORT jobjectArray JNICALL Java_java_lang_Class_getSignatureAnnotation(JNIEnv* env, jobject obj) {
		throw VmException("Java_java_lang_Class_getSignatureAnnotation not implemented!");
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_getDeclaredMethodInternal(JNIEnv* env, jobject obj, jstring name, jobjectArray args) {
		throw VmException("Java_java_lang_Class_getDeclaredMethodInternal not implemented!");
	}

}  // extern "C"
