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

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "loader/dex/Annotation.hpp"
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
		return (jobject)sandvik::Object::makeConstClass(classloader, objclass);
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_newInstance(JNIEnv* env, jobject obj) {
		auto classObj = sandvik::native::getObject(obj);
		const Class& targetClass = classObj->getClassType();

		logger.debug(fmt::format("Class.newInstance: Creating new instance of {}", targetClass.getFullname()));
		jclass jtargetClass = (jclass)Object::make(const_cast<Class&>(targetClass));

		jmethodID ctorID = env->GetMethodID(jtargetClass, "<init>", "()V");
		if (!ctorID) {
			throw NoSuchMethodException(fmt::format("Class.newInstance: default constructor <init>()V not found in {}", targetClass.getFullname()));
		}
		return env->NewObject(jtargetClass, ctorID);
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
		auto jenv = sandvik::native::getNativeInterface(env);
		auto classObj = sandvik::native::getObject(obj);
		if (!classObj || classObj->isNull()) {
			return (jobject)Object::makeNull();
		}

		auto& classloader = jenv->getClassLoader();
		auto& clazz = classObj->getClassType();

		const auto* enclosingClass = clazz.getAnnotation("dalvik.annotation.EnclosingClass");
		if (enclosingClass) {
			const auto* value = enclosingClass->element("value");
			if (value && value->kind() == dex::EncodedValue::Kind::Type) {
				auto& declaringClass = classloader.getOrLoad(value->asString());
				return (jobject)Object::makeConstClass(classloader, declaringClass);
			}
		}

		// Fallback for classes with no EnclosingClass annotation (e.g. hand-built test dex).
		const auto& name = clazz.getFullname();
		auto pos = name.rfind('$');
		if (pos == std::string::npos) {
			return (jobject)Object::makeNull();
		}

		auto declaringName = name.substr(0, pos);
		logger.fdebug("[Class.getDeclaringClass] {} -> {}", name, declaringName);
		auto& declaringClass = classloader.getOrLoad(declaringName);

		return (jobject)Object::makeConstClass(classloader, declaringClass);
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_getEnclosingClass(JNIEnv* env, jobject obj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto classObj = sandvik::native::getObject(obj);
		if (!classObj || classObj->isNull()) {
			return (jobject)Object::makeNull();
		}

		auto& classloader = jenv->getClassLoader();
		auto& clazz = classObj->getClassType();

		const auto* enclosingClass = clazz.getAnnotation("dalvik.annotation.EnclosingClass");
		if (!enclosingClass) {
			return (jobject)Object::makeNull();
		}
		const auto* value = enclosingClass->element("value");
		if (!value || value->kind() != dex::EncodedValue::Kind::Type) {
			return (jobject)Object::makeNull();
		}

		auto& enclosing = classloader.getOrLoad(value->asString());
		return (jobject)Object::makeConstClass(classloader, enclosing);
	}

	JNIEXPORT jboolean JNICALL Java_java_lang_Class_isAnonymousClass(JNIEnv* env, jobject obj) {
		auto classObj = sandvik::native::getObject(obj);
		if (!classObj || classObj->isNull()) {
			return JNI_FALSE;
		}

		auto& cls = classObj->getClassType();
		const std::string& name = cls.getFullname();

		// Anonymous classes generated by javac/d8 typically look like: com.foo.Bar$1
		// Check whether the suffix after the last '$' is entirely numeric.
		auto pos = name.rfind('$');
		if (pos == std::string::npos || pos + 1 >= name.size()) {
			return JNI_FALSE;
		}
		for (size_t i = pos + 1; i < name.size(); ++i) {
			if (!std::isdigit(static_cast<unsigned char>(name[i]))) {
				return JNI_FALSE;
			}
		}
		return JNI_TRUE;
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_getPublicFieldRecursive(JNIEnv* env, jobject obj, jstring name) {
		throw VmException("Java_java_lang_Class_getPublicFieldRecursive not implemented!");
	}

	JNIEXPORT jobjectArray JNICALL Java_java_lang_Class_getDeclaredClasses(JNIEnv* env, jobject obj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto& classloader = jenv->getClassLoader();
		auto classObj = sandvik::native::getObject(obj);
		auto& classType = classObj->getClassType();
		auto& classArrayType = classloader.getOrLoad("java/lang/Class");

		const auto* memberClasses = classType.getAnnotation("dalvik.annotation.MemberClasses");
		if (!memberClasses) {
			return (jobjectArray)Array::make(classArrayType, 0u);
		}
		const auto* value = memberClasses->element("value");
		if (!value || value->kind() != dex::EncodedValue::Kind::Array) {
			return (jobjectArray)Array::make(classArrayType, 0u);
		}

		const auto& items = value->asArray();
		auto array = Array::make(classArrayType, static_cast<uint32_t>(items.size()));
		for (size_t i = 0; i < items.size(); ++i) {
			auto& memberClass = classloader.getOrLoad(items[i].asString());
			array->setElement(static_cast<uint32_t>(i), Object::makeConstClass(classloader, memberClass));
		}
		return (jobjectArray)array;
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
		auto& fieldTypeClass = classloader.getOrLoad(field.getType());
		fieldObj->setField("type", Object::makeConstClass(classloader, fieldTypeClass));
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

		const Class& targetClass = classObj->getClassType();
		logger.debug(fmt::format("Class.getDeclaredConstructor: Getting constructor for class {}", targetClass.getFullname()));

		// Create a java.lang.reflect.Constructor object
		auto& ctorClass = classloader.getOrLoad("java/lang/reflect/Constructor");
		auto ctorObj = sandvik::Object::make(ctorClass);

		// Set fields as needed (e.g., 'declaringClass', 'parameterTypes', etc.)
		ctorObj->setField("declaringClass", Object::makeConstClass(classloader, const_cast<Class&>(targetClass)));
		// For simplicity, parameterTypes is empty for default constructor
		ctorObj->setField("parameterTypes", sandvik::Object::make(classloader.getOrLoad("java/lang/Object")));

		return (jobject)ctorObj;
	}

	JNIEXPORT jstring JNICALL Java_java_lang_Class_getInnerClassName(JNIEnv* env, jobject obj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto classObj = sandvik::native::getObject(obj);
		auto& classloader = jenv->getClassLoader();
		auto& classType = classObj->getClassType();

		const auto* innerClass = classType.getAnnotation("dalvik.annotation.InnerClass");
		if (!innerClass) {
			return (jstring)Object::makeNull();
		}
		const auto* name = innerClass->element("name");
		if (!name || name->kind() != dex::EncodedValue::Kind::String) {
			return (jstring)Object::makeNull();
		}
		return (jstring)Object::make(classloader, name->asString());
	}

	JNIEXPORT jint JNICALL Java_java_lang_Class_getInnerClassFlags(JNIEnv* env, jobject obj, jint defaultValue) {
		(void)env;
		auto classObj = sandvik::native::getObject(obj);
		auto& classType = classObj->getClassType();

		const auto* innerClass = classType.getAnnotation("dalvik.annotation.InnerClass");
		if (!innerClass) {
			return defaultValue;
		}
		const auto* accessFlags = innerClass->element("accessFlags");
		if (!accessFlags || accessFlags->kind() != dex::EncodedValue::Kind::Int) {
			return defaultValue;
		}
		return static_cast<jint>(accessFlags->asInt());
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_getDeclaredAnnotation(JNIEnv* env, jobject obj, jobject annotationClass) {
		throw VmException("Java_java_lang_Class_getDeclaredAnnotation not implemented!");
	}

	JNIEXPORT jobjectArray JNICALL Java_java_lang_Class_getDeclaredAnnotations(JNIEnv* env, jobject obj) {
		throw VmException("Java_java_lang_Class_getDeclaredAnnotations not implemented!");
	}

	JNIEXPORT jboolean JNICALL Java_java_lang_Class_isDeclaredAnnotationPresent(JNIEnv* env, jobject obj, jobject annotationClass) {
		auto classObj = sandvik::native::getObject(obj);
		auto annotationClassObj = sandvik::native::getObject(annotationClass);
		if (!annotationClassObj || annotationClassObj->isNull()) {
			return JNI_FALSE;
		}
		auto& classType = classObj->getClassType();
		auto& annotationType = annotationClassObj->getClassType();
		return classType.hasAnnotation(annotationType.getFullname()) ? JNI_TRUE : JNI_FALSE;
	}

	JNIEXPORT jobjectArray JNICALL Java_java_lang_Class_getSignatureAnnotation(JNIEnv* env, jobject obj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto classObj = sandvik::native::getObject(obj);
		auto& classloader = jenv->getClassLoader();
		auto& classType = classObj->getClassType();
		auto& stringClass = classloader.getOrLoad("java/lang/String");

		const auto* signature = classType.getAnnotation("dalvik.annotation.Signature");
		if (!signature) {
			return (jobjectArray)Object::makeNull();
		}
		const auto* value = signature->element("value");
		if (!value || value->kind() != dex::EncodedValue::Kind::Array) {
			return (jobjectArray)Object::makeNull();
		}

		const auto& items = value->asArray();
		auto array = Array::make(stringClass, static_cast<uint32_t>(items.size()));
		for (size_t i = 0; i < items.size(); ++i) {
			array->setElement(static_cast<uint32_t>(i), Object::make(classloader, items[i].asString()));
		}
		return (jobjectArray)array;
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_getDeclaredMethodInternal(JNIEnv* env, jobject obj, jstring name, jobjectArray args) {
		throw VmException("Java_java_lang_Class_getDeclaredMethodInternal not implemented!");
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_getComponentType(JNIEnv* env, jobject obj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto classObj = sandvik::native::getObject(obj);
		auto& classloader = jenv->getClassLoader();
		auto& classType = classObj->getClassType();

		// Check if the class is an array
		if (!classType.isArray()) {
			return (jobject)Object::makeNull();
		}
		auto componentType = classObj->getField("componentType");
		if (componentType->isNull()) {
			auto& t = classloader.getOrLoad(classType.getArrayType());
			auto componentClassObj = sandvik::Object::makeConstClass(classloader, t);
			classObj->setField("componentType", componentClassObj);
			return (jobject)componentClassObj;
		}
		return (jobject)componentType;
	}

	JNIEXPORT jobject JNICALL Java_java_lang_Class_getSuperclass(JNIEnv* env, jobject obj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto classObj = sandvik::native::getObject(obj);
		auto& classType = classObj->getClassType();
		if (!classType.hasSuperClass()) {
			return (jobject)Object::makeNull();
		}
		auto& classloader = jenv->getClassLoader();
		auto& superClass = classType.getSuperClass();
		return (jobject)Object::makeConstClass(classloader, superClass);
	}

	JNIEXPORT jobjectArray JNICALL Java_java_lang_Class_getEnumConstantsShared(JNIEnv* env, jobject obj) {
		(void)env;
		auto classObj = sandvik::native::getObject(obj);
		const auto& enumType = classObj->getClassType();

		if (enumType.hasField("$VALUES")) {
			auto values = enumType.getField("$VALUES").getObjectValue();
			if (!values->isNull()) {
				return (jobjectArray)values;
			}
		}
		if (enumType.hasField("ENUM$VALUES")) {
			auto values = enumType.getField("ENUM$VALUES").getObjectValue();
			if (!values->isNull()) {
				return (jobjectArray)values;
			}
		}

		// Fallback: return an empty array with the concrete enum component type.
		return (jobjectArray)Array::make(enumType, 0);
	}
}  // extern "C"
