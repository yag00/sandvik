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
#include <sys/stat.h>

#include "ResourcesJni.hpp"
#include "system/logger.hpp"
#include "vfs.hpp"

using namespace sandvik;
using namespace sandvik::resources;
using namespace sandvik::resources::jni;

extern "C" {

	JNIEXPORT jlong JNICALL Java_android_content_res_ApkAssets_nativeLoad(JNIEnv* env, jclass, jint format, jstring path, jint, jobject) {
		const auto assetPath = toString(env, path);
		const auto hostPath = VFS::resolve(assetPath);
		struct stat st;
		if (stat(hostPath.c_str(), &st) != 0) {
			throw IOException("Failed to load asset path " + assetPath);
		}
		try {
			return ApkAssetsRegistry::instance().add(ApkAssets::load(hostPath, static_cast<ApkAssets::Format>(format), assetPath));
		} catch (const VmException& e) {
			logger.ferror("{}", e.what());
			throw IOException("Failed to load asset path " + assetPath);
		}
	}

	JNIEXPORT jlong JNICALL Java_android_content_res_ApkAssets_nativeLoadEmpty(JNIEnv*, jclass, jint, jobject) {
		return ApkAssetsRegistry::instance().add(ApkAssets::loadEmpty("empty"));
	}

	JNIEXPORT jlong JNICALL Java_android_content_res_ApkAssets_nativeLoadFd(JNIEnv* env, jclass, jint, jobject, jstring friendlyName, jint, jobject) {
		throw IOException("ApkAssets.loadFromFd is not supported: " + toString(env, friendlyName));
	}

	JNIEXPORT jlong JNICALL Java_android_content_res_ApkAssets_nativeLoadFdOffsets(JNIEnv* env, jclass, jint, jobject, jstring friendlyName, jlong, jlong, jint,
	                                                                               jobject) {
		throw IOException("ApkAssets.loadFromFd is not supported: " + toString(env, friendlyName));
	}

	JNIEXPORT void JNICALL Java_android_content_res_ApkAssets_nativeDestroy(JNIEnv*, jclass, jlong ptr) {
		ApkAssetsRegistry::instance().remove(ptr);
	}

	JNIEXPORT jstring JNICALL Java_android_content_res_ApkAssets_nativeGetAssetPath(JNIEnv* env, jclass, jlong ptr) {
		return newString(env, lookup<ApkAssets>(ptr, "ApkAssets.getAssetPath")->getAssetPath());
	}

	JNIEXPORT jstring JNICALL Java_android_content_res_ApkAssets_nativeGetDebugName(JNIEnv* env, jclass, jlong ptr) {
		return newString(env, lookup<ApkAssets>(ptr, "ApkAssets.getDebugName")->getAssetPath());
	}

	// The returned block is not owned by Java (StringBlock(ptr, false)): it stays registered while the ApkAssets lives
	JNIEXPORT jlong JNICALL Java_android_content_res_ApkAssets_nativeGetStringBlock(JNIEnv*, jclass, jlong ptr) {
		auto pool = lookup<ApkAssets>(ptr, "ApkAssets.getStringBlock")->getStringPool();
		if (!pool) {
			return 0;
		}
		return StringPoolRegistry::instance().add(pool);
	}

	JNIEXPORT jboolean JNICALL Java_android_content_res_ApkAssets_nativeIsUpToDate(JNIEnv*, jclass, jlong) {
		return JNI_TRUE;
	}

	JNIEXPORT jlong JNICALL Java_android_content_res_ApkAssets_nativeOpenXml(JNIEnv* env, jclass, jlong, jstring fileName) {
		// binary XML (XmlBlock) is not implemented yet
		logger.fwarning("ApkAssets.openXml not implemented: {}", toString(env, fileName));
		throw IOException("Failed to open XML file " + toString(env, fileName));
	}

	JNIEXPORT jobject JNICALL Java_android_content_res_ApkAssets_nativeGetOverlayableInfo(JNIEnv*, jclass, jlong, jstring) {
		return nullptr;
	}

	JNIEXPORT jboolean JNICALL Java_android_content_res_ApkAssets_nativeDefinesOverlayable(JNIEnv*, jclass, jlong) {
		return JNI_FALSE;
	}

}  // extern "C"
