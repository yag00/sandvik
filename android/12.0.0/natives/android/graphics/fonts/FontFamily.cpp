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

#include <string>

#include "NativeFont.hpp"

using sandvik::fonts::fromHandle;
using sandvik::fonts::NativeFont;
using sandvik::fonts::NativeFontFamily;
using sandvik::fonts::toHandle;

namespace {
	void releaseFamily(void* family_) {
		delete static_cast<NativeFontFamily*>(family_);
	}
}  // namespace

extern "C" {

	// android.graphics.fonts.FontFamily$Builder: fonts are copied, the Java Font keeps ownership of its own native font

	JNIEXPORT jlong JNICALL Java_android_graphics_fonts_FontFamily_00024Builder_nInitBuilder(JNIEnv*, jclass) {
		return toHandle(new NativeFontFamily());
	}

	JNIEXPORT void JNICALL Java_android_graphics_fonts_FontFamily_00024Builder_nAddFont(JNIEnv*, jclass, jlong builderPtr, jlong fontPtr) {
		fromHandle<NativeFontFamily>(builderPtr)->fonts.push_back(*fromHandle<NativeFont>(fontPtr));
	}

	JNIEXPORT jlong JNICALL Java_android_graphics_fonts_FontFamily_00024Builder_nBuild(JNIEnv* env, jclass, jlong builderPtr, jstring langTags, jint variant,
	                                                                                   jboolean) {
		auto* family = fromHandle<NativeFontFamily>(builderPtr);
		if (langTags != nullptr) {
			const char* utf8 = env->GetStringUTFChars(langTags, nullptr);
			family->langTags = utf8;
			env->ReleaseStringUTFChars(langTags, utf8);
		}
		family->variant = variant;
		return builderPtr;
	}

	JNIEXPORT jlong JNICALL Java_android_graphics_fonts_FontFamily_00024Builder_nGetReleaseNativeFamily(JNIEnv*, jclass) {
		return toHandle(reinterpret_cast<void*>(&releaseFamily));
	}

	// android.graphics.fonts.FontFamily

	JNIEXPORT jint JNICALL Java_android_graphics_fonts_FontFamily_nGetFontSize(JNIEnv*, jclass, jlong familyPtr) {
		return (jint)fromHandle<NativeFontFamily>(familyPtr)->fonts.size();
	}

	// Returns a new native font owned by the Java Font wrapping it
	JNIEXPORT jlong JNICALL Java_android_graphics_fonts_FontFamily_nGetFont(JNIEnv*, jclass, jlong familyPtr, jint index) {
		return toHandle(new NativeFont(fromHandle<NativeFontFamily>(familyPtr)->fonts.at(index)));
	}

	JNIEXPORT jstring JNICALL Java_android_graphics_fonts_FontFamily_nGetLangTags(JNIEnv* env, jclass, jlong familyPtr) {
		return env->NewStringUTF(fromHandle<NativeFontFamily>(familyPtr)->langTags.c_str());
	}

	JNIEXPORT jint JNICALL Java_android_graphics_fonts_FontFamily_nGetVariant(JNIEnv*, jclass, jlong familyPtr) {
		return fromHandle<NativeFontFamily>(familyPtr)->variant;
	}

}  // extern "C"
