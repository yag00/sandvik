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

#include "annotation_factory.hpp"
#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "jni.hpp"
#include "jthread.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "vm.hpp"

/** @todo implementation **/

extern "C" {

	JNIEXPORT jobject JNICALL Java_java_lang_reflect_Field_get(JNIEnv* env, jobject obj, jobject receiver) {
		auto fieldObj = sandvik::native::getObject(obj);
		auto nameObj = fieldObj->getField("name");
		const std::string& fieldName = nameObj->str();
		auto declaringClassObj = fieldObj->getField("declaringClass");
		auto& clazz = const_cast<sandvik::Class&>(declaringClassObj->getClassType());
		auto& field = clazz.getField(fieldName);
		auto& classloader = sandvik::native::getNativeInterface(env)->getClassLoader();
		const auto type = field.getType();
		const bool primitive = type.size() == 1;
		if (field.isStatic()) {
			// the receiver is ignored for a static field, whose class is initialized on first access
			sandvik::native::getNativeInterface(env)->getVm().currentThread().ensureInitialized(field.getClass());
			if (!primitive) {
				return (jobject)field.getObjectValue();
			}
			const uint64_t value = (type == "J" || type == "D") ? field.getLongValue() : field.getIntValue();
			return (jobject)sandvik::native::boxPrimitive(classloader, type[0], value);
		}
		auto receiverObj = sandvik::native::getReferenceObjectOrNull(receiver);
		if (receiverObj == nullptr || receiverObj->isNull()) {
			throw sandvik::NullPointerException(fmt::format("Field.get: null receiver for instance field {}", fieldName));
		}
		auto fieldValue = receiverObj->getField(fieldName);
		if (primitive) {
			return (jobject)sandvik::native::boxPrimitive(classloader, type[0], static_cast<uint64_t>(fieldValue->getLongValue()));
		}
		return (jobject)fieldValue;
	}

	// Primitive fields receive boxed values (Integer, Long...): the field is set from their "value" field
	JNIEXPORT void JNICALL Java_java_lang_reflect_Field_set(JNIEnv* env, jobject obj, jobject receiver, jobject value) {
		auto fieldObj = sandvik::native::getObject(obj);
		const std::string& fieldName = fieldObj->getField("name")->str();
		auto declaringClassObj = fieldObj->getField("declaringClass");
		auto& clazz = const_cast<sandvik::Class&>(declaringClassObj->getClassType());
		auto& field = clazz.getField(fieldName);
		const auto type = field.getType();
		const bool primitive = type.size() == 1;

		auto valueObj = sandvik::native::getReferenceObjectOrNull(value);
		if (valueObj == nullptr) {
			if (primitive) {
				throw sandvik::IllegalArgumentException(fmt::format("Field.set: null value for primitive field {}", fieldName));
			}
			valueObj = sandvik::Object::makeNull();
		} else if (primitive) {
			valueObj = valueObj->getField("value");
		}

		auto receiverObj = sandvik::native::getReferenceObjectOrNull(receiver);
		if (field.isStatic()) {
			sandvik::native::getNativeInterface(env)->getVm().currentThread().ensureInitialized(field.getClass());
			if (!primitive) {
				field.setObjectValue(valueObj);
			} else if (type == "J" || type == "D") {
				field.setLongValue(valueObj->getLongValue());
			} else {
				field.setIntValue(static_cast<uint32_t>(valueObj->getValue()));
			}
			return;
		}
		if (receiverObj == nullptr || receiverObj->isNull()) {
			throw sandvik::NullPointerException(fmt::format("Field.set: null receiver for instance field {}", fieldName));
		}
		receiverObj->setField(fieldName, valueObj);
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

	JNIEXPORT jobject JNICALL Java_java_lang_reflect_Field_getAnnotationNative(JNIEnv* env, jobject obj, jobject annotationType) {
		auto fieldObj = sandvik::native::getObject(obj);
		auto annotationTypeObj = sandvik::native::getReferenceObjectOrNull(annotationType);
		if (annotationTypeObj == nullptr || annotationTypeObj->isNull()) {
			return nullptr;
		}
		auto& clazz = const_cast<sandvik::Class&>(fieldObj->getField("declaringClass")->getClassType());
		auto& field = clazz.getField(fieldObj->getField("name")->str());
		const auto* annotation = field.getAnnotation(annotationTypeObj->getClassType().getFullname());
		if (annotation == nullptr) {
			return nullptr;
		}
		return (jobject)sandvik::makeAnnotationInstance(sandvik::native::getNativeInterface(env)->getVm().currentThread(), *annotation);
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
