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

#include <algorithm>
#include <cstdint>

#include "system/logger.hpp"

using namespace sandvik;

namespace {
	// Font-less stand-in for hwui's Typeface: only keeps what the Java side reads back
	struct NativeTypeface {
			int apiStyle;    // Typeface.NORMAL, BOLD, ITALIC, BOLD_ITALIC
			int baseWeight;  // weight before applying the bold style
			int weight;      // resolved weight, 1..1000
			bool italic;
	};

	constexpr int BOLD = 1;
	constexpr int ITALIC = 2;
	constexpr int NORMAL_WEIGHT = 400;
	constexpr int BOLD_WEIGHT_DELTA = 300;
	constexpr int RESOLVE_BY_FONT_TABLE = -1;

	NativeTypeface* sDefault = nullptr;

	NativeTypeface* resolve(jlong handle_) {
		auto* typeface = reinterpret_cast<NativeTypeface*>(static_cast<uintptr_t>(handle_));
		if (typeface != nullptr) {
			return typeface;
		}
		if (sDefault == nullptr) {
			sDefault = new NativeTypeface{0, NORMAL_WEIGHT, NORMAL_WEIGHT, false};
		}
		return sDefault;
	}

	jlong toHandle(NativeTypeface* typeface_) {
		return static_cast<jlong>(reinterpret_cast<uintptr_t>(typeface_));
	}

	jlong makeAbsolute(int baseWeight_, int weight_, bool italic_) {
		const int weight = std::clamp(weight_, 1, 1000);
		const int apiStyle = (weight >= 600 ? BOLD : 0) | (italic_ ? ITALIC : 0);
		return toHandle(new NativeTypeface{apiStyle, baseWeight_, weight, italic_});
	}

	void releaseTypeface(void* typeface_) {
		auto* typeface = static_cast<NativeTypeface*>(typeface_);
		if (typeface != sDefault) {
			delete typeface;
		}
	}
}  // namespace

extern "C" {

	JNIEXPORT jlong JNICALL Java_android_graphics_Typeface_nativeCreateFromTypeface(JNIEnv*, jclass, jlong src, jint style) {
		const auto* base = resolve(src);
		const int weight = std::min(base->baseWeight + ((style & BOLD) ? BOLD_WEIGHT_DELTA : 0), 1000);
		return toHandle(new NativeTypeface{style & (BOLD | ITALIC), base->baseWeight, weight, (style & ITALIC) != 0});
	}

	JNIEXPORT jlong JNICALL Java_android_graphics_Typeface_nativeCreateFromTypefaceWithExactStyle(JNIEnv*, jclass, jlong src, jint weight, jboolean italic) {
		return makeAbsolute(resolve(src)->baseWeight, weight, italic == JNI_TRUE);
	}

	JNIEXPORT jlong JNICALL Java_android_graphics_Typeface_nativeCreateFromTypefaceWithVariation(JNIEnv*, jclass, jlong src, jobject) {
		return toHandle(new NativeTypeface(*resolve(src)));
	}

	JNIEXPORT jlong JNICALL Java_android_graphics_Typeface_nativeCreateWeightAlias(JNIEnv*, jclass, jlong src, jint weight) {
		auto* typeface = new NativeTypeface(*resolve(src));
		typeface->baseWeight = weight;
		typeface->weight = weight;
		return toHandle(typeface);
	}

	JNIEXPORT jlong JNICALL Java_android_graphics_Typeface_nativeCreateFromArray(JNIEnv*, jclass, jlongArray, jlong, jint weight, jint italic) {
		const int resolvedWeight = weight == RESOLVE_BY_FONT_TABLE ? NORMAL_WEIGHT : weight;
		return makeAbsolute(resolvedWeight, resolvedWeight, italic == 1);
	}

	JNIEXPORT jintArray JNICALL Java_android_graphics_Typeface_nativeGetSupportedAxes(JNIEnv*, jclass, jlong) {
		return nullptr;
	}

	JNIEXPORT void JNICALL Java_android_graphics_Typeface_nativeSetDefault(JNIEnv*, jclass, jlong handle) {
		sDefault = resolve(handle);
	}

	JNIEXPORT jint JNICALL Java_android_graphics_Typeface_nativeGetStyle(JNIEnv*, jclass, jlong handle) {
		return resolve(handle)->apiStyle;
	}

	JNIEXPORT jint JNICALL Java_android_graphics_Typeface_nativeGetWeight(JNIEnv*, jclass, jlong handle) {
		return resolve(handle)->weight;
	}

	JNIEXPORT jlong JNICALL Java_android_graphics_Typeface_nativeGetReleaseFunc(JNIEnv*, jclass) {
		return static_cast<jlong>(reinterpret_cast<uintptr_t>(&releaseTypeface));
	}

	JNIEXPORT jint JNICALL Java_android_graphics_Typeface_nativeGetFamilySize(JNIEnv*, jclass, jlong) {
		return 0;
	}

	JNIEXPORT jlong JNICALL Java_android_graphics_Typeface_nativeGetFamily(JNIEnv*, jclass, jlong, jint) {
		return 0;
	}

	JNIEXPORT void JNICALL Java_android_graphics_Typeface_nativeRegisterGenericFamily(JNIEnv*, jclass, jstring, jlong) {
	}

	JNIEXPORT jint JNICALL Java_android_graphics_Typeface_nativeWriteTypefaces(JNIEnv*, jclass, jobject, jlongArray) {
		logger.fwarning("{} not implemented!", __FUNCTION__);
		return 0;
	}

	JNIEXPORT jlongArray JNICALL Java_android_graphics_Typeface_nativeReadTypefaces(JNIEnv*, jclass, jobject) {
		logger.fwarning("{} not implemented!", __FUNCTION__);
		return nullptr;
	}

	JNIEXPORT void JNICALL Java_android_graphics_Typeface_nativeForceSetStaticFinalField(JNIEnv* env, jclass clazz, jstring fieldName, jobject typeface) {
		const char* name = env->GetStringUTFChars(fieldName, nullptr);
		jfieldID fid = env->GetStaticFieldID(clazz, name, "Landroid/graphics/Typeface;");
		env->ReleaseStringUTFChars(fieldName, name);
		env->SetStaticObjectField(clazz, fid, typeface);
	}

	JNIEXPORT void JNICALL Java_android_graphics_Typeface_nativeAddFontCollections(JNIEnv*, jclass, jlong) {
	}

	JNIEXPORT void JNICALL Java_android_graphics_Typeface_nativeWarmUpCache(JNIEnv*, jclass, jstring) {
	}

}  // extern "C"
