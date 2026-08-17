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

#include "apk_assets_handle.hpp"
#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "version.hpp"

using namespace sandvik;

extern "C" {
	JNIEXPORT jlong JNICALL Java_android_content_res_AssetManager_nativeGetThemeFreeFunction(JNIEnv* env, jclass clazz) {
		(void)env;
		(void)clazz;

		// Sandvik does not have a native theme resource to free, so we return a no-op function pointer.
		static auto themeFreeFn = +[](void*) {
			// No-op function, does nothing.
		};
		return reinterpret_cast<jlong>(static_cast<void (*)(void*)>(themeFreeFn));
	}

	JNIEXPORT jlong JNICALL Java_android_content_res_ApkAssets_nativeLoad(JNIEnv* env, jclass clazz, jint format, jstring path, jint flags, jobject assets) {
		(void)env;
		(void)clazz;
		(void)format;
		(void)flags;
		(void)assets;

		std::string pathStr;
		if (auto obj = sandvik::native::getNullableString(path)) {
			pathStr = obj->str();
		}

		jlong cookie = sandvik::ApkAssetsRegistry::instance().create(pathStr);
		logger.fwarning("ApkAssets.nativeLoad stub cookie={} path={}", cookie, pathStr);
		return cookie;
	}

	JNIEXPORT void JNICALL Java_android_content_res_ApkAssets_nativeClose(JNIEnv* env, jclass clazz, jlong cookie) {
		(void)env;
		(void)clazz;
		if (!sandvik::ApkAssetsRegistry::instance().destroy(cookie)) {
			logger.fwarning("ApkAssets.nativeClose: unknown cookie {}", cookie);
		}
	}
}  // extern "C"