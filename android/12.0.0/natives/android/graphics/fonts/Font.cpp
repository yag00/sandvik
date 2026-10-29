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

#include <atomic>
#include <cstring>
#include <string>

#include "NativeFont.hpp"
#include "system/logger.hpp"

using namespace sandvik;
using sandvik::fonts::fromHandle;
using sandvik::fonts::NativeFont;
using sandvik::fonts::toHandle;

namespace {
	// Font.Builder state between nInitBuilder and nBuild/nClone
	struct FontBuilder {
			std::vector<std::pair<int, float>> axes;
	};

	std::atomic<int> sNextSourceId{1};

	void releaseFont(void* font_) {
		delete static_cast<NativeFont*>(font_);
	}

	std::string toString(JNIEnv* env_, jstring str_) {
		if (str_ == nullptr) {
			return "";
		}
		const char* utf8 = env_->GetStringUTFChars(str_, nullptr);
		std::string result(utf8);
		env_->ReleaseStringUTFChars(str_, utf8);
		return result;
	}
}  // namespace

extern "C" {

	// android.graphics.fonts.Font$Builder

	JNIEXPORT jlong JNICALL Java_android_graphics_fonts_Font_00024Builder_nInitBuilder(JNIEnv*, jclass) {
		return toHandle(new FontBuilder());
	}

	JNIEXPORT void JNICALL Java_android_graphics_fonts_Font_00024Builder_nAddAxis(JNIEnv*, jclass, jlong builderPtr, jint tag, jfloat value) {
		fromHandle<FontBuilder>(builderPtr)->axes.emplace_back(tag, value);
	}

	JNIEXPORT jlong JNICALL Java_android_graphics_fonts_Font_00024Builder_nBuild(JNIEnv* env, jclass, jlong builderPtr, jobject buffer, jstring filePath,
	                                                                             jstring localeList, jint weight, jboolean italic, jint ttcIndex) {
		auto* builder = fromHandle<FontBuilder>(builderPtr);
		auto* font = new NativeFont();
		font->path = toString(env, filePath);
		font->localeList = toString(env, localeList);
		font->weight = weight;
		font->italic = italic == JNI_TRUE;
		font->ttcIndex = ttcIndex;
		font->axes = std::move(builder->axes);
		font->buffer = env->GetDirectBufferAddress(buffer);
		font->bufferSize = env->GetDirectBufferCapacity(buffer);
		font->sourceId = sNextSourceId++;
		delete builder;
		return toHandle(font);
	}

	JNIEXPORT jlong JNICALL Java_android_graphics_fonts_Font_00024Builder_nGetReleaseNativeFont(JNIEnv*, jclass) {
		return toHandle(reinterpret_cast<void*>(&releaseFont));
	}

	JNIEXPORT jlong JNICALL Java_android_graphics_fonts_Font_00024Builder_nClone(JNIEnv*, jclass, jlong fontPtr, jlong builderPtr, jint weight, jboolean italic,
	                                                                             jint ttcIndex) {
		auto* builder = fromHandle<FontBuilder>(builderPtr);
		auto* font = new NativeFont(*fromHandle<NativeFont>(fontPtr));
		font->weight = weight;
		font->italic = italic == JNI_TRUE;
		font->ttcIndex = ttcIndex;
		if (!builder->axes.empty()) {
			font->axes = std::move(builder->axes);
		}
		delete builder;
		return toHandle(font);
	}

	// android.graphics.fonts.Font

	JNIEXPORT jlong JNICALL Java_android_graphics_fonts_Font_nGetMinikinFontPtr(JNIEnv*, jclass, jlong fontPtr) {
		return fontPtr;
	}

	JNIEXPORT jlong JNICALL Java_android_graphics_fonts_Font_nCloneFont(JNIEnv*, jclass, jlong fontPtr) {
		return toHandle(new NativeFont(*fromHandle<NativeFont>(fontPtr)));
	}

	JNIEXPORT jobject JNICALL Java_android_graphics_fonts_Font_nNewByteBuffer(JNIEnv* env, jclass, jlong fontPtr) {
		const auto* font = fromHandle<NativeFont>(fontPtr);
		return env->NewDirectByteBuffer(font->buffer, font->bufferSize);
	}

	JNIEXPORT jlong JNICALL Java_android_graphics_fonts_Font_nGetBufferAddress(JNIEnv*, jclass, jlong fontPtr) {
		return toHandle(fromHandle<NativeFont>(fontPtr)->buffer);
	}

	JNIEXPORT jint JNICALL Java_android_graphics_fonts_Font_nGetSourceId(JNIEnv*, jclass, jlong fontPtr) {
		return fromHandle<NativeFont>(fontPtr)->sourceId;
	}

	JNIEXPORT jlong JNICALL Java_android_graphics_fonts_Font_nGetReleaseNativeFont(JNIEnv*, jclass) {
		return toHandle(reinterpret_cast<void*>(&releaseFont));
	}

	JNIEXPORT jfloat JNICALL Java_android_graphics_fonts_Font_nGetGlyphBounds(JNIEnv*, jclass, jlong, jint, jlong, jobject) {
		logger.fwarning("{} not implemented!", __FUNCTION__);
		return 0.0f;
	}

	JNIEXPORT jfloat JNICALL Java_android_graphics_fonts_Font_nGetFontMetrics(JNIEnv*, jclass, jlong, jlong, jobject) {
		logger.fwarning("{} not implemented!", __FUNCTION__);
		return 0.0f;
	}

	JNIEXPORT jstring JNICALL Java_android_graphics_fonts_Font_nGetFontPath(JNIEnv* env, jclass, jlong fontPtr) {
		const auto* font = fromHandle<NativeFont>(fontPtr);
		return font->path.empty() ? nullptr : env->NewStringUTF(font->path.c_str());
	}

	JNIEXPORT jstring JNICALL Java_android_graphics_fonts_Font_nGetLocaleList(JNIEnv* env, jclass, jlong fontPtr) {
		return env->NewStringUTF(fromHandle<NativeFont>(fontPtr)->localeList.c_str());
	}

	// Same packing as minikin FontStyle: weight in the low 16 bits, slant (1 = italic) above
	JNIEXPORT jint JNICALL Java_android_graphics_fonts_Font_nGetPackedStyle(JNIEnv*, jclass, jlong fontPtr) {
		const auto* font = fromHandle<NativeFont>(fontPtr);
		return font->weight | (font->italic ? (1 << 16) : 0);
	}

	JNIEXPORT jint JNICALL Java_android_graphics_fonts_Font_nGetIndex(JNIEnv*, jclass, jlong fontPtr) {
		return fromHandle<NativeFont>(fontPtr)->ttcIndex;
	}

	JNIEXPORT jint JNICALL Java_android_graphics_fonts_Font_nGetAxisCount(JNIEnv*, jclass, jlong fontPtr) {
		return (jint)fromHandle<NativeFont>(fontPtr)->axes.size();
	}

	// Packed as tag << 32 | float bits of the value
	JNIEXPORT jlong JNICALL Java_android_graphics_fonts_Font_nGetAxisInfo(JNIEnv*, jclass, jlong fontPtr, jint index) {
		const auto& [tag, value] = fromHandle<NativeFont>(fontPtr)->axes.at(index);
		uint32_t bits = 0;
		std::memcpy(&bits, &value, sizeof(bits));
		return (static_cast<jlong>(static_cast<uint32_t>(tag)) << 32) | bits;
	}

	JNIEXPORT jlongArray JNICALL Java_android_graphics_fonts_Font_nGetAvailableFontSet(JNIEnv* env, jclass) {
		return env->NewLongArray(0);
	}

}  // extern "C"
