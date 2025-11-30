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

#ifndef __NATIVE_UTILS_HPP__
#define __NATIVE_UTILS_HPP__

#include <string>

#include "jni/jni.h"

namespace sandvik {
	class Object;
	class Array;
	class NativeInterface;
	/** @brief Native function utilities */
	namespace native {
		/** @brief Retrieves a native Object from a Java jobject. */
		Object* getObject(jobject jobj);
		/** @brief Retrieves a native Object from a Java jstring. */
		Object* getString(jobject jstr);
		/** @brief Retrieves a native Object from a Java jstring. */
		Object* getString(jstring jstr);
		/** @brief Retrieves a native Array from a Java jobject. */
		Array* getArray(jobject jarray);
		/** @brief Retrieves the native interface from the Java environment. */
		NativeInterface* getNativeInterface(JNIEnv* env);
	};  // namespace native
};  // namespace sandvik

#endif /* __NATIVE_UTILS_HPP__ */
