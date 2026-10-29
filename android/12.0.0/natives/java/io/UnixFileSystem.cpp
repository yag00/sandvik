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
#include <fcntl.h>
#include <unistd.h>

#include <fmt/format.h>
#include <jni/jni.h>
#include <sys/stat.h>

#include <cerrno>
#include <cstring>

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "vfs.hpp"

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
		std::string path = VFS::resolve(pathObj->str());

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

	// private native int getBooleanAttributes0(String abspath)
	JNIEXPORT jint JNICALL Java_java_io_UnixFileSystem_getBooleanAttributes0(JNIEnv* env, jobject obj, jstring abspath) {
		// java.io.FileSystem BA_* flags
		constexpr jint BA_EXISTS = 0x01;
		constexpr jint BA_REGULAR = 0x02;
		constexpr jint BA_DIRECTORY = 0x04;
		constexpr jint BA_HIDDEN = 0x08;

		const std::string& path = sandvik::native::getString(abspath)->str();
		struct stat st;
		if (stat(VFS::resolve(path).c_str(), &st) != 0) {
			return 0;
		}
		jint attributes = BA_EXISTS;
		if (S_ISREG(st.st_mode)) {
			attributes |= BA_REGULAR;
		} else if (S_ISDIR(st.st_mode)) {
			attributes |= BA_DIRECTORY;
		}
		const auto slash = path.find_last_of('/');
		if (path[slash == std::string::npos ? 0 : slash + 1] == '.') {
			attributes |= BA_HIDDEN;
		}
		return attributes;
	}

	JNIEXPORT jstring JNICALL Java_java_io_UnixFileSystem_canonicalize0(JNIEnv* env, jobject obj, jstring path) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto& classloader = jenv->getClassLoader();

		auto pathStr = sandvik::native::getObject(path);
		char resolvedPath[PATH_MAX];
		if (realpath(pathStr->str().c_str(), resolvedPath) != nullptr) {
			return (jstring)Object::make(classloader, resolvedPath);
		}
		return path;
	}

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

	JNIEXPORT jboolean JNICALL Java_java_io_UnixFileSystem_createDirectory0(JNIEnv* env, jobject obj, jobject fileObj) {
		const auto path = VFS::resolve(sandvik::native::getObject(fileObj)->getField("path")->str());
		return ::mkdir(path.c_str(), 0777) == 0 ? JNI_TRUE : JNI_FALSE;
	}

	// Throws IOException on failure, false if the file already exists
	JNIEXPORT jboolean JNICALL Java_java_io_UnixFileSystem_createFileExclusively0(JNIEnv* env, jobject obj, jstring pathStr) {
		const auto path = VFS::resolve(sandvik::native::getString(pathStr)->str());
		const int fd = ::open(path.c_str(), O_RDWR | O_CREAT | O_EXCL, 0666);
		if (fd < 0) {
			if (errno == EEXIST) {
				return JNI_FALSE;
			}
			throw IOException(fmt::format("{}: {}", path, strerror(errno)));
		}
		::close(fd);
		return JNI_TRUE;
	}

	// Milliseconds since the epoch, 0 if the file does not exist
	JNIEXPORT jlong JNICALL Java_java_io_UnixFileSystem_getLastModifiedTime0(JNIEnv* env, jobject obj, jobject fileObj) {
		const auto path = VFS::resolve(sandvik::native::getObject(fileObj)->getField("path")->str());
		struct stat st;
		if (stat(path.c_str(), &st) != 0) {
			return 0;
		}
		return static_cast<jlong>(st.st_mtim.tv_sec) * 1000 + st.st_mtim.tv_nsec / 1000000;
	}

}  // extern "C"
