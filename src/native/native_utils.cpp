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

#include "native_utils.hpp"

#include <fmt/format.h>

#include "exceptions.hpp"
#include "jni.hpp"
#include "object.hpp"

using namespace sandvik;

Object* native::getObject(jobject jobj) {
	Object* ptr = (Object*)jobj;
	if (ptr == nullptr) {
		throw NullPointerException("null object");
	}
	return ptr;
}

Object* native::getString(jobject jstr) {
	return native::getString((jstring)jstr);
}

Object* native::getString(jstring jstr) {
	Object* ptr = (Object*)jstr;
	if (ptr == nullptr) {
		throw NullPointerException("null object");
	}
	if (!ptr->isString()) {
		throw ClassCastException("Object is not a java.lang.String");
	}
	return ptr;
}

NativeInterface* native::getNativeInterface(JNIEnv* env) {
	NativeInterface* jenv = static_cast<NativeInterface*>(env);
	if (jenv == nullptr) {
		throw std::runtime_error("Internal error: JNIEnv is not a NativeInterface");
	}
	return jenv;
}