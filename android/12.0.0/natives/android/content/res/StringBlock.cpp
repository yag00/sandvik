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

#include <vector>

#include "ResourcesJni.hpp"

using namespace sandvik::resources;
using namespace sandvik::resources::jni;

extern "C" {

	JNIEXPORT jlong JNICALL Java_android_content_res_StringBlock_nativeCreate(JNIEnv* env, jclass, jbyteArray data, jint offset, jint size) {
		std::vector<jbyte> bytes(size);
		env->GetByteArrayRegion(data, offset, size, bytes.data());
		return StringPoolRegistry::instance().add(StringPool::fromBytes(reinterpret_cast<const uint8_t*>(bytes.data()), bytes.size()));
	}

	JNIEXPORT jint JNICALL Java_android_content_res_StringBlock_nativeGetSize(JNIEnv*, jclass, jlong ptr) {
		return static_cast<jint>(lookup<StringPool>(ptr, "StringBlock.getSize")->size());
	}

	JNIEXPORT jstring JNICALL Java_android_content_res_StringBlock_nativeGetString(JNIEnv* env, jclass, jlong ptr, jint index) {
		auto str = lookup<StringPool>(ptr, "StringBlock.getString")->stringAt(static_cast<uint32_t>(index));
		return str ? newString(env, *str) : nullptr;
	}

	// Style spans as (tag string index, first char, last char) triples, null if the string has no style
	JNIEXPORT jintArray JNICALL Java_android_content_res_StringBlock_nativeGetStyle(JNIEnv* env, jclass, jlong ptr, jint index) {
		auto spans = lookup<StringPool>(ptr, "StringBlock.getStyle")->styleAt(static_cast<uint32_t>(index));
		if (spans.empty()) {
			return nullptr;
		}
		std::vector<int32_t> values;
		for (const auto& span : spans) {
			values.push_back(static_cast<int32_t>(span.name));
			values.push_back(static_cast<int32_t>(span.firstChar));
			values.push_back(static_cast<int32_t>(span.lastChar));
		}
		return newIntArray(env, values);
	}

	JNIEXPORT void JNICALL Java_android_content_res_StringBlock_nativeDestroy(JNIEnv*, jclass, jlong ptr) {
		StringPoolRegistry::instance().remove(ptr);
	}

}  // extern "C"
