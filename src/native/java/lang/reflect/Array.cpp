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

#include "array.hpp"

#include <fmt/format.h>
#include <jni/jni.h>

#include "class.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "jnihandlemap.hpp"
#include "native/native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

extern "C" {
	JNIEXPORT jobject JNICALL Java_java_lang_reflect_Array_multiNewArray(JNIEnv* env, jclass, jobject classObj, jobject dimsArr) {
		auto jenv = sandvik::native::getNativeInterface(env);

		auto obj = sandvik::native::getObject(classObj);
		auto dimArray = (sandvik::Array*)sandvik::native::getObject(dimsArr);

		logger.fdebug("Creating multi-dimensional array of type: {}", obj->getClassType().getFullname());

		// Get dimensions from dimsArr (assume it's a Java int array)
		std::vector<uint32_t> dimensions;
		logger.fdebug("Array dimensions: {}", dimArray->getArrayLength());
		for (size_t i = 0; i < dimArray->getArrayLength(); ++i) {
			auto element = dimArray->getElement(i);
			if (!element->isNumberObject()) {
				throw std::runtime_error(fmt::format("Dimension at index {} is not a NumberObject", i));
			}
			logger.fdebug(" - dim[{}] = {}", i, element->getValue());
			dimensions.push_back(element->getValue());
		}

		// Create the multi-dimensional array
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		auto arrObj = sandvik::Object::makeArray(classloader, obj->getClassType(), dimensions);
		// Return as jobject
		return jenv->getHandles().toJObject(arrObj);
	}
}
