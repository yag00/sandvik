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

#include <array>
#include <cstdint>

namespace {
	// Stand-in for SkColorSpace: transfer function parameters and the XYZ D50 matrix
	struct NativeColorSpace {
			std::array<float, 7> transfer;  // a, b, c, d, e, f, g
			std::array<float, 9> toXYZD50;
	};

	void releaseColorSpace(void* colorSpace_) {
		delete static_cast<NativeColorSpace*>(colorSpace_);
	}
}  // namespace

extern "C" {

	JNIEXPORT jlong JNICALL Java_android_graphics_ColorSpace_00024Rgb_nativeGetNativeFinalizer(JNIEnv*, jclass) {
		return static_cast<jlong>(reinterpret_cast<uintptr_t>(&releaseColorSpace));
	}

	JNIEXPORT jlong JNICALL Java_android_graphics_ColorSpace_00024Rgb_nativeCreate(JNIEnv* env, jclass, jfloat a, jfloat b, jfloat c, jfloat d, jfloat e,
	                                                                               jfloat f, jfloat g, jfloatArray xyzD50) {
		auto* colorSpace = new NativeColorSpace{{a, b, c, d, e, f, g}, {}};
		env->GetFloatArrayRegion(xyzD50, 0, (jsize)colorSpace->toXYZD50.size(), colorSpace->toXYZD50.data());
		return static_cast<jlong>(reinterpret_cast<uintptr_t>(colorSpace));
	}

}  // extern "C"
