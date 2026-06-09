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
#include "array.hpp"

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
	JNIEXPORT jobject JNICALL Java_java_lang_reflect_Array_createMultiArray(JNIEnv* env, jclass, jobject componentType, jintArray dimensions) {
		auto compClass = native::getObject(componentType);
		auto array = native::getArray(dimensions);
		if (array->getArrayLength() == 0) {
			throw VmException("Array dimensions cannot be empty");
		}

		std::vector<uint32_t> dims;
		for (size_t i = 0; i < array->getArrayLength(); ++i) {
			auto dim = array->getElement(i)->getValue();
			if (dim <= 0) {
				throw VmException("Array dimension cannot <= 0");
			}
			dims.push_back(dim);
		}

		auto newArray = Array::make(compClass->getClass(), dims);
		return (jobject)newArray;
	}

}  // extern "C"
