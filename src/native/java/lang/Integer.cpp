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
	JNIEXPORT jint JNICALL Java_java_lang_Integer_parseInt__Ljava_lang_String_2I(JNIEnv* env, jclass clazz, jstring str, jint radix) {
		auto objstr = sandvik::native::getString(str);
		try {
			int value = std::stoi(objstr->str(), nullptr, radix);
			return static_cast<jint>(value);
		} catch (const std::exception& e) {
			throw sandvik::NumberFormatException(fmt::format("Failed to parse '{}' to integer value with radix {}: {}", objstr->str(), radix, e.what()));
		}
	}
}