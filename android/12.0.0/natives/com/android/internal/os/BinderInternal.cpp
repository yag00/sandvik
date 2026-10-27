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

#include <jni/jni.h>

#include "classloader.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"

extern "C" {

	JNIEXPORT jobject JNICALL Java_com_android_internal_os_BinderInternal_getContextObject(JNIEnv* env, jclass) {
		auto& classloader = sandvik::native::getNativeInterface(env)->getClassLoader();
		return (jobject)sandvik::Object::make(classloader.getOrLoad("android/os/Binder"));
	}

}  // extern "C"
