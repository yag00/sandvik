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

#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "jnihandlemap.hpp"
#include "native/native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

extern "C" {
	JNIEXPORT jobject JNICALL Java_java_lang_StringBuilder_append__Ljava_lang_String_2(JNIEnv* env, jobject obj, jstring str) {
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();

		auto this_ptr = sandvik::native::getObject(obj);
		auto nullstring = sandvik::Object::make(classloader, "null");
		auto objstr = sandvik::native::getObject(str)->isNull() ? nullstring.get() : sandvik::native::getString(str);
		if (this_ptr->getField("string")->isNull()) {
			this_ptr->setField("string", sandvik::Object::make(classloader, objstr->str()));
		} else {
			auto current = this_ptr->getField("string");
			if (!current->isString()) {
				throw sandvik::ClassCastException("Field 'string' is not a StringObject");
			}
			this_ptr->setField("string", sandvik::Object::make(classloader, current->str() + objstr->str()));
		}
		return obj;
	}

	JNIEXPORT jobject JNICALL Java_java_lang_StringBuilder_append__I(JNIEnv* env, jobject obj, jint i) {
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();

		auto this_ptr = sandvik::native::getObject(obj);
		std::string intStr = std::to_string(i);
		if (this_ptr->getField("string")->isNull()) {
			this_ptr->setField("string", sandvik::Object::make(classloader, intStr));
		} else {
			auto current = this_ptr->getField("string");
			if (!current->isString()) {
				throw sandvik::ClassCastException("Field 'string' is not a StringObject");
			}
			this_ptr->setField("string", sandvik::Object::make(classloader, current->str() + intStr));
		}
		return obj;
	}
	JNIEXPORT jobject JNICALL Java_java_lang_StringBuilder_append__F(JNIEnv* env, jobject obj, jfloat f) {
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();

		auto this_ptr = sandvik::native::getObject(obj);
		std::string floatStr = std::to_string(f);
		if (this_ptr->getField("string")->isNull()) {
			this_ptr->setField("string", sandvik::Object::make(classloader, floatStr));
		} else {
			auto current = this_ptr->getField("string");
			if (!current->isString()) {
				throw sandvik::ClassCastException("Field 'string' is not a StringObject");
			}
			this_ptr->setField("string", sandvik::Object::make(classloader, current->str() + floatStr));
		}
		return obj;
	}
	JNIEXPORT jobject JNICALL Java_java_lang_StringBuilder_append__D(JNIEnv* env, jobject obj, jdouble d) {
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();

		auto this_ptr = sandvik::native::getObject(obj);
		std::string doubleStr = std::to_string(d);
		if (this_ptr->getField("string")->isNull()) {
			this_ptr->setField("string", sandvik::Object::make(classloader, doubleStr));
		} else {
			auto current = this_ptr->getField("string");
			if (!current->isString()) {
				throw sandvik::ClassCastException("Field 'string' is not a StringObject");
			}
			this_ptr->setField("string", sandvik::Object::make(classloader, current->str() + doubleStr));
		}
		return obj;
	}

	JNIEXPORT jobject JNICALL Java_java_lang_StringBuilder_append__J(JNIEnv* env, jobject obj, jlong j) {
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();

		auto this_ptr = sandvik::native::getObject(obj);
		std::string longStr = std::to_string(j);
		if (this_ptr->getField("string")->isNull()) {
			this_ptr->setField("string", sandvik::Object::make(classloader, longStr));
		} else {
			auto current = this_ptr->getField("string");
			if (!current->isString()) {
				throw sandvik::ClassCastException("Field 'string' is not a StringObject");
			}
			this_ptr->setField("string", sandvik::Object::make(classloader, current->str() + longStr));
		}
		return obj;
	}

	JNIEXPORT jobject JNICALL Java_java_lang_StringBuilder_toString(JNIEnv* env, jobject obj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();

		auto this_ptr = sandvik::native::getObject(obj);
		if (this_ptr->getField("string")->isNull()) {
			this_ptr->setField("string", sandvik::Object::make(classloader, ""));
		}
		auto current = this_ptr->getField("string");
		if (!current->isString()) {
			throw sandvik::ClassCastException("Field 'string' is not a StringObject");
		}
		auto strObj = sandvik::Object::make(classloader, current->str());
		jobject jstr = jenv->getHandles().toJObject(strObj);
		return jstr;
	}
}