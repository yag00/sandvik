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

#include "field.hpp"

#include <fmt/format.h>
#include <jni/jni.h>

#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "jni.hpp"
#include "native/native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

extern "C" {
	JNIEXPORT jobject JNICALL Java_java_lang_reflect_Field_get(JNIEnv* env, jobject fieldObj, jobject targetObj) {
		auto field = sandvik::native::getObject(fieldObj);
		auto target = sandvik::native::getObject(targetObj);
		// Get the field name from the Field object
		auto nameObj = field->getField("name");
		std::string fieldName = nameObj->str();
		// Get the value of the field from the target object
		auto value = target->getField(fieldName);
		// Return the value as a jobject
		return (jobject)value;
	}
}