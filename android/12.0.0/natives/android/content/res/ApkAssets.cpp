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
#include "string_block_handle.hpp"
#include "system/logger.hpp"
#include "version.hpp"

using namespace sandvik;

extern "C" {

	JNIEXPORT jlong JNICALL Java_android_content_res_ApkAssets_nativeGetStringBlock(JNIEnv* env, jclass clazz, jlong ptr) {
		(void)env;
		(void)clazz;

		auto* apkHandle = sandvik::ApkAssetsRegistry::instance().get(ptr);
		if (!apkHandle) {
			logger.fwarning("ApkAssets.nativeGetStringBlock: unknown ApkAssets cookie {}", ptr);
			throw VmException("nativeGetStringBlock: invalid ApkAssets handle");
		}

		jlong cookie = sandvik::StringBlockRegistry::instance().create(ptr);
		logger.fwarning("ApkAssets.nativeGetStringBlock stub: apkAssets={} -> stringBlock={} (path={})", ptr, cookie, apkHandle->path);
		return cookie;
	}
}  // extern "C"