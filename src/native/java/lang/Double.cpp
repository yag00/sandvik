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
#include "field.hpp"
#include "jni.hpp"
#include "object.hpp"
#include "system/logger.hpp"

extern "C" {
	JNIEXPORT jboolean JNICALL Java_java_lang_Double_isNaN__D(JNIEnv *, jclass, jdouble value) {
		return std::isnan(value) ? JNI_TRUE : JNI_FALSE;
	}
	JNIEXPORT jlong JNICALL Java_java_lang_Double_doubleToRawLongBits(JNIEnv *, jclass, jdouble value) {
		// Convert the double value to its raw long bit representation
		return std::bit_cast<jlong>(value);
	}
	JNIEXPORT jdouble JNICALL Java_java_lang_Double_longBitsToDouble(JNIEnv *, jclass, jlong value) {
		// Convert the raw long bit representation to a double value
		return std::bit_cast<jdouble>(value);
	}
}