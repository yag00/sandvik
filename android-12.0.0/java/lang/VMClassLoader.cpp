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

#include <cstdlib>
#include <filesystem>

#include "array.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

namespace {
	std::mutex g_cacheMutex;
	std::unordered_map<std::string, std::string> g_extractedCache;

	std::string cacheRoot() {
		return "/tmp/sandvik-res-cache";
	}

	std::string writeToCache(const std::string& name, const std::vector<uint8_t>& data) {
		std::lock_guard<std::mutex> lock(g_cacheMutex);

		auto it = g_extractedCache.find(name);
		if (it != g_extractedCache.end()) {
			return it->second;
		}

		const std::string root = cacheRoot();
		std::error_code ec;
		std::filesystem::create_directories(root, ec);
		if (ec) {
			logger.ferror("Failed to create root cache directory '{}': {}", root, ec.message());
			return "";
		}

		// Since "name" can contain slashes, create directories for the full path to the cache file
		std::filesystem::path fullPath = std::filesystem::path(root) / name;
		std::filesystem::create_directories(fullPath.parent_path(), ec);
		if (ec) {
			logger.ferror("Failed to create cache tree '{}': {}", fullPath.parent_path().string(), ec.message());
			return "";
		}
		std::ofstream out(fullPath, std::ios::binary | std::ios::trunc);
		if (!out) {
			logger.ferror("Failed to write resource '{}' to {}", name, fullPath.string());
			return "";
		}

		out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
		out.close();

		g_extractedCache[name] = fullPath.string();
		return fullPath.string();
	}
}  // namespace

extern "C" {

	JNIEXPORT jobjectArray JNICALL Java_java_lang_VMClassLoader_getBootClassPathEntries(JNIEnv* env, jclass) {
		auto jenv = native::getNativeInterface(env);
		auto& classloader = jenv->getClassLoader();
		const auto& entries = classloader.getClassPathEntries();
		auto& stringClass = classloader.getOrLoad("java/lang/String");
		auto array = Array::make(stringClass, static_cast<uint32_t>(entries.size()));
		for (size_t i = 0; i < entries.size(); ++i) {
			array->setElement(static_cast<uint32_t>(i), Object::make(classloader, entries[i]));
		}
		return (jobjectArray)array;
	}

	JNIEXPORT jobject JNICALL Java_java_lang_VMClassLoader_findLoadedClass(JNIEnv* env, jclass, jobject loader, jstring name) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto& classloader = jenv->getClassLoader();

		auto nameObj = sandvik::native::getString(name);
		std::string classname = nameObj->str();

		if (!classloader.isLoaded(classname)) {
			return nullptr;
		}

		auto& cls = classloader.getOrLoad(classname);
		return (jobject)Object::makeConstClass(classloader, cls);
	}

	JNIEXPORT jobject JNICALL Java_java_lang_VMClassLoader_getResource(JNIEnv* env, jclass, jstring name) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto& classloader = jenv->getClassLoader();
		auto nameObj = sandvik::native::getString(name);
		std::string resourceName = nameObj->str();

		logger.fdebug("VMClassLoader.getResource(\"{}\")", resourceName);
		auto data = classloader.findResource(resourceName);
		if (!data) {
			logger.fdebug("Resource '{}' not found in any loaded JAR", resourceName);
			return nullptr;
		}

		std::string extractedPath = writeToCache(resourceName, *data);
		if (extractedPath.empty()) {
			return nullptr;
		}

		std::string urlStr = "file://" + extractedPath;
		jclass urlClass = env->FindClass("java/net/URL");
		if (urlClass == nullptr) return nullptr;

		jmethodID ctor = env->GetMethodID(urlClass, "<init>", "(Ljava/lang/String;)V");
		jstring jUrlStr = env->NewStringUTF(urlStr.c_str());
		jobject urlObj = env->NewObject(urlClass, ctor, jUrlStr);

		if (env->ExceptionCheck()) {
			env->ExceptionDescribe();
			env->ExceptionClear();
			return nullptr;
		}

		return urlObj;
	}

}  // extern "C"
