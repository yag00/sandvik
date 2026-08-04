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

#include <dirent.h>

#include <fmt/format.h>
#include <jni/jni.h>

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

/** @todo implementation **/

using namespace sandvik;

extern "C" {

	JNIEXPORT jobjectArray JNICALL Java_java_io_UnixFileSystem_list0(JNIEnv* env, jobject thiz, jobject fileObj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto& classloader = jenv->getClassLoader();

		auto file = sandvik::native::getObject(fileObj);
		auto pathObj = file->getField("path");
		if (!pathObj->isString()) {
			throw VmException("UnixFileSystem.list0: File.path is not a string");
		}
		std::string path = pathObj->str();

		DIR* dir = opendir(path.c_str());
		if (dir == nullptr) {
			return nullptr;
		}

		std::vector<std::string> entries;
		struct dirent* entry;
		while ((entry = readdir(dir)) != nullptr) {
			std::string name = entry->d_name;
			if (name == "." || name == "..") {
				continue;
			}
			entries.push_back(name);
		}
		closedir(dir);

		auto& stringClass = classloader.getOrLoad("java/lang/String");
		auto array = Array::make(stringClass, static_cast<uint32_t>(entries.size()));
		for (size_t i = 0; i < entries.size(); ++i) {
			array->setElement(static_cast<uint32_t>(i), Object::make(classloader, entries[i]));
		}
		return (jobjectArray)array;
	}

#if 0
JNIEXPORT void JNICALL Java_java_io_UnixFileSystem_getBooleanAttributes0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_io_UnixFileSystem_getLastModifiedTime0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_io_UnixFileSystem_setPermission0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_io_UnixFileSystem_delete0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_io_UnixFileSystem_createDirectory0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_io_UnixFileSystem_rename0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_io_UnixFileSystem_setLastModifiedTime0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_io_UnixFileSystem_setReadOnly0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_io_UnixFileSystem_getSpace0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_io_UnixFileSystem_initIDs(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

}  // extern "C"
