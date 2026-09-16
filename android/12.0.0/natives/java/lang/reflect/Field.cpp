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

#include "field.hpp"

#include <fmt/format.h>
#include <jni/jni.h>

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

/** @todo implementation **/

extern "C" {

	JNIEXPORT jobject JNICALL Java_java_lang_reflect_Field_get(JNIEnv* env, jobject obj, jobject receiver) {
		auto fieldObj = sandvik::native::getObject(obj);
		auto nameObj = fieldObj->getField("name");
		const std::string& fieldName = nameObj->str();
		auto receiverObj = sandvik::native::getObject(receiver);
		if (receiverObj == nullptr || receiverObj->isNull()) {
			// static field access: resolve via declaringClass
			auto declaringClassObj = fieldObj->getField("declaringClass");
			auto& clazz = const_cast<sandvik::Class&>(declaringClassObj->getClassType());
			auto& field = clazz.getField(fieldName);
			return (jobject)field.getObjectValue();
		}
		auto fieldValue = receiverObj->getField(fieldName);
		return (jobject)fieldValue;
	}

#if 0
JNIEXPORT void JNICALL Java_java_lang_reflect_Field_getNameInternal(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

	JNIEXPORT jobjectArray JNICALL Java_java_lang_reflect_Field_getSignatureAnnotation(JNIEnv* env, jobject obj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto& classloader = jenv->getClassLoader();
		auto fieldObj = sandvik::native::getObject(obj);
		auto declaringClassObj = fieldObj->getField("declaringClass");
		auto& clazz = const_cast<sandvik::Class&>(declaringClassObj->getClassType());
		auto nameObj = fieldObj->getField("name");
		auto& field = clazz.getField(nameObj->str());

		auto& stringClass = classloader.getOrLoad("java/lang/String");
		const auto* signature = field.getAnnotation("dalvik.annotation.Signature");
		if (!signature) {
			return (jobjectArray)sandvik::Object::makeNull();
		}
		const auto* value = signature->element("value");
		if (!value || value->kind() != sandvik::dex::EncodedValue::Kind::Array) {
			return (jobjectArray)sandvik::Object::makeNull();
		}

		const auto& items = value->asArray();
		auto array = sandvik::Array::make(stringClass, static_cast<uint32_t>(items.size()));
		for (size_t i = 0; i < items.size(); ++i) {
			array->setElement(static_cast<uint32_t>(i), sandvik::Object::make(classloader, items[i].asString()));
		}
		return (jobjectArray)array;
	}

	JNIEXPORT jboolean JNICALL Java_java_lang_reflect_Field_isAnnotationPresentNative(JNIEnv* env, jobject obj, jobject annotationType) {
		auto fieldObj = sandvik::native::getObject(obj);
		auto annotationTypeObj = sandvik::native::getObject(annotationType);
		if (!annotationTypeObj || annotationTypeObj->isNull()) {
			return JNI_FALSE;
		}
		auto declaringClassObj = fieldObj->getField("declaringClass");
		auto& clazz = const_cast<sandvik::Class&>(declaringClassObj->getClassType());
		auto nameObj = fieldObj->getField("name");
		auto& field = clazz.getField(nameObj->str());
		auto& annotationClass = annotationTypeObj->getClassType();
		return field.hasAnnotation(annotationClass.getFullname()) ? JNI_TRUE : JNI_FALSE;
	}

#if 0
JNIEXPORT void JNICALL Java_java_lang_reflect_Field_getDeclaredAnnotations(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_reflect_Field_getArtField(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

}  // extern "C"
