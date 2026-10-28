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

#include <jni/jni.h>

#include <cmath>

// java.lang.StrictMath natives map to the C library (rint uses the default round-to-nearest-even mode)

extern "C" {

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_acos(JNIEnv*, jclass, jdouble a) {
		return std::acos(a);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_asin(JNIEnv*, jclass, jdouble a) {
		return std::asin(a);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_atan(JNIEnv*, jclass, jdouble a) {
		return std::atan(a);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_atan2(JNIEnv*, jclass, jdouble a, jdouble b) {
		return std::atan2(a, b);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_cbrt(JNIEnv*, jclass, jdouble a) {
		return std::cbrt(a);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_cos(JNIEnv*, jclass, jdouble a) {
		return std::cos(a);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_cosh(JNIEnv*, jclass, jdouble a) {
		return std::cosh(a);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_exp(JNIEnv*, jclass, jdouble a) {
		return std::exp(a);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_expm1(JNIEnv*, jclass, jdouble a) {
		return std::expm1(a);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_hypot(JNIEnv*, jclass, jdouble a, jdouble b) {
		return std::hypot(a, b);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_IEEEremainder(JNIEnv*, jclass, jdouble a, jdouble b) {
		return std::remainder(a, b);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_log(JNIEnv*, jclass, jdouble a) {
		return std::log(a);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_log10(JNIEnv*, jclass, jdouble a) {
		return std::log10(a);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_log1p(JNIEnv*, jclass, jdouble a) {
		return std::log1p(a);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_pow(JNIEnv*, jclass, jdouble a, jdouble b) {
		return std::pow(a, b);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_sin(JNIEnv*, jclass, jdouble a) {
		return std::sin(a);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_sinh(JNIEnv*, jclass, jdouble a) {
		return std::sinh(a);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_sqrt(JNIEnv*, jclass, jdouble a) {
		return std::sqrt(a);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_tan(JNIEnv*, jclass, jdouble a) {
		return std::tan(a);
	}

	JNIEXPORT jdouble JNICALL Java_java_lang_StrictMath_tanh(JNIEnv*, jclass, jdouble a) {
		return std::tanh(a);
	}

}  // extern "C"
