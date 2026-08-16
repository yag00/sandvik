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

#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>

#include <fmt/format.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/utsname.h>

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/env_var.hpp"
#include "system/logger.hpp"
#include "vfs.hpp"

using namespace sandvik;

extern "C" {
	JNIEXPORT jstring JNICALL Java_libcore_io_Linux_getenv(JNIEnv* env, jclass, jstring name) {
		auto key = sandvik::native::getString(name);
		auto value = sandvik::system::env::get(key->str());
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		return (jstring)sandvik::Object::make(classloader, std::string(value));
	}
	JNIEXPORT jint JNICALL Java_libcore_io_Linux_getuid(JNIEnv* env, jclass) {
		return static_cast<jint>(getuid());
	}
	JNIEXPORT jobject JNICALL Java_libcore_io_Linux_getpwuid(JNIEnv* env, jclass, jint uid) {
		struct passwd* pwd = getpwuid(static_cast<uid_t>(uid));
		if (!pwd) {
			return nullptr;
		}
		jclass structPasswdClass = env->FindClass("android/system/StructPasswd");
		jstring name = env->NewStringUTF(pwd->pw_name ? pwd->pw_name : "");
		jstring dir = env->NewStringUTF(pwd->pw_dir ? pwd->pw_dir : "");
		jstring shell = env->NewStringUTF(pwd->pw_shell ? pwd->pw_shell : "");
		jmethodID ctor = env->GetMethodID(structPasswdClass, "<init>", "(Ljava/lang/String;IILjava/lang/String;Ljava/lang/String;)V");
		jobject result = env->NewObject(structPasswdClass, ctor, name, static_cast<jint>(pwd->pw_uid), static_cast<jint>(pwd->pw_gid), dir, shell);
		return result;
	}
	JNIEXPORT jobject JNICALL Java_libcore_io_Linux_uname(JNIEnv* env, jclass) {
		struct utsname buf;
		if (uname(&buf) != 0) {
			return nullptr;
		}
		jclass structUtsnameClass = env->FindClass("android/system/StructUtsname");
		jstring sysname = env->NewStringUTF(buf.sysname);
		jstring nodename = env->NewStringUTF(buf.nodename);
		jstring release = env->NewStringUTF(buf.release);
		jstring version = env->NewStringUTF(buf.version);
		jstring machine = env->NewStringUTF(buf.machine);
		jmethodID ctor =
		    env->GetMethodID(structUtsnameClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
		jobject result = env->NewObject(structUtsnameClass, ctor, sysname, nodename, release, version, machine);
		return result;
	}
	JNIEXPORT jlong JNICALL Java_libcore_io_Linux_sysconf(JNIEnv* env, jclass, jint name) {
		errno = 0;
		logger.fdebug("call sysconf(name={})", name);
		long result = 0;  // sysconf(name);
		if (result == -1 && errno != 0) {
			logger.ferror("sysconf failed with error: {}", strerror(errno));
			return -1;
		}
		return static_cast<jlong>(result);
	}
	JNIEXPORT jint JNICALL Java_libcore_io_Linux_writeBytes(JNIEnv* env, jclass, jobject fdObj, jbyteArray byteArray, jint offset, jint count) {
		auto fdObject = sandvik::native::getObject(fdObj);
		auto buffer = sandvik::native::getArray(byteArray);

		int fd = fdObject->getField("descriptor")->getValue();
		if (fd < 0) {
			throw IOException("Bad file descriptor");
		}
		if (offset < 0 || count < 0) {
			throw IllegalArgumentException("offset or byteCount < 0");
		}
		if (static_cast<size_t>(offset) + static_cast<size_t>(count) > static_cast<size_t>(buffer->getArrayLength())) {
			throw ArrayIndexOutOfBoundsException();
		}

		uint8_t* data = new uint8_t[count];
		for (int i = 0; i < count; ++i) {
			data[i] = static_cast<uint8_t>(buffer->getElement(offset + i)->getValue());
		}
		ssize_t written = ::write(fd, data, count);
		delete[] data;

		if (written < 0) {
			throw IOException(fmt::format("Write operation failed: {}", strerror(errno)));
		}

		return static_cast<jint>(written);
	}

	JNIEXPORT jint JNICALL Java_libcore_io_Linux_readBytes(JNIEnv* env, jclass, jobject fdObj, jobject bufferObj, jint offset, jint count) {
		auto fdObject = sandvik::native::getObject(fdObj);
		auto buffer = sandvik::native::getArray(bufferObj);

		int fd = fdObject->getField("descriptor")->getValue();
		if (fd < 0) {
			throw IOException("Bad file descriptor");
		}
		if (offset < 0 || count < 0) {
			throw IllegalArgumentException("offset or byteCount < 0");
		}
		if (static_cast<size_t>(offset) + static_cast<size_t>(count) > static_cast<size_t>(buffer->getArrayLength())) {
			throw ArrayIndexOutOfBoundsException();
		}

		std::vector<uint8_t> data(count);
		ssize_t nread = ::read(fd, data.data(), count);
		if (nread < 0) {
			throw IOException(fmt::format("Read operation failed: {}", strerror(errno)));
		}
		for (ssize_t i = 0; i < nread; ++i) {
			buffer->setElement(offset + static_cast<int>(i), Object::make(static_cast<int32_t>(data[i])));
		}

		return static_cast<jint>(nread);
	}

	// Places a process into a process group, used during zygote and child process setup.
	JNIEXPORT void JNICALL Java_libcore_io_Linux_setpgid(JNIEnv* env, jobject thiz, jint pid, jint pgid) {
		(void)env;
		(void)thiz;
		(void)setpgid(static_cast<pid_t>(pid), static_cast<pid_t>(pgid));
	}

	JNIEXPORT jint JNICALL Java_libcore_io_Linux_getgid(JNIEnv* env, jclass) {
		return static_cast<jint>(getgid());
	}

	JNIEXPORT jobject JNICALL Java_libcore_io_Linux_open(JNIEnv* env, jclass clazz, jstring path, jint flags, jint mode) {
		const char* pathCStr = env->GetStringUTFChars(path, nullptr);
		std::string realPath = VFS::resolve(pathCStr);
		int fd = ::open(realPath.c_str(), flags, mode);
		int openErrno = errno;
		env->ReleaseStringUTFChars(path, pathCStr);
		if (fd == -1) {
			throw IOException(fmt::format("open failed on '{}' (flags={:#x}, mode={:#o}): {}", realPath, flags, mode, strerror(openErrno)));
		}

		jclass fileDescriptorClass = env->FindClass("java/io/FileDescriptor");
		jmethodID constructor = env->GetMethodID(fileDescriptorClass, "<init>", "()V");
		jobject fileDescriptorObj = env->NewObject(fileDescriptorClass, constructor);

		jfieldID descriptorField = env->GetFieldID(fileDescriptorClass, "descriptor", "I");
		env->SetIntField(fileDescriptorObj, descriptorField, fd);
		logger.fdebug("Linux.open('{}') -> fd={}", realPath, fd);

		return fileDescriptorObj;
	}

	JNIEXPORT jlong JNICALL Java_libcore_io_Linux_mmap(JNIEnv* env, jclass clazz, jlong address, jlong length, jint prot, jint flags, jobject fdObj,
	                                                   jlong offset) {
		auto fdObject = sandvik::native::getObject(fdObj);
		int fd = fdObject->getField("descriptor")->getValue();

		void* result = ::mmap(reinterpret_cast<void*>(address), static_cast<size_t>(length), prot, flags, fd, static_cast<off_t>(offset));
		if (result == MAP_FAILED) {
			throw IOException(fmt::format("mmap failed: {}", strerror(errno)));
		}
		return reinterpret_cast<jlong>(result);
	}

	JNIEXPORT jint JNICALL Java_libcore_io_Linux_munmap(JNIEnv* env, jclass clazz, jlong address, jlong length) {
		int result = ::munmap(reinterpret_cast<void*>(address), static_cast<size_t>(length));
		if (result == -1) {
			throw IOException(fmt::format("munmap failed: {}", strerror(errno)));
		}
		return result;
	}

	JNIEXPORT jobject JNICALL Java_libcore_io_Linux_fstat(JNIEnv* env, jclass clazz, jobject fdObj) {
		auto fdObject = sandvik::native::getObject(fdObj);
		int fd = fdObject->getField("descriptor")->getValue();

		struct stat st;
		logger.fdebug("Linux.fstat(fd={})", fd);
		if (::fstat(fd, &st) == -1) {
			throw IOException(fmt::format("fstat failed: {}", strerror(errno)));
		}

		jclass structStatClass = env->FindClass("android/system/StructStat");
		jmethodID ctor = env->GetMethodID(structStatClass, "<init>", "(JJIJIIJJJJJJJ)V");
		return env->NewObject(structStatClass, ctor, static_cast<jlong>(st.st_dev), static_cast<jlong>(st.st_ino), static_cast<jint>(st.st_mode),
		                      static_cast<jlong>(st.st_nlink), static_cast<jint>(st.st_uid), static_cast<jint>(st.st_gid), static_cast<jlong>(st.st_rdev),
		                      static_cast<jlong>(st.st_size), static_cast<jlong>(st.st_atime), static_cast<jlong>(st.st_mtime), static_cast<jlong>(st.st_ctime),
		                      static_cast<jlong>(st.st_blksize), static_cast<jlong>(st.st_blocks));
	}

	JNIEXPORT jobject JNICALL Java_libcore_io_Linux_stat(JNIEnv* env, jclass clazz, jstring path) {
		const char* pathCStr = env->GetStringUTFChars(path, nullptr);
		std::string realPath = VFS::resolve(pathCStr);
		struct stat st;
		logger.fdebug("Linux.stat('{}')", realPath);
		if (::stat(realPath.c_str(), &st) == -1) {
			throw IOException(fmt::format("stat failed on '{}': {}", realPath, strerror(errno)));
		}
		env->ReleaseStringUTFChars(path, pathCStr);

		jclass structStatClass = env->FindClass("android/system/StructStat");
		jmethodID ctor = env->GetMethodID(structStatClass, "<init>", "(JJIJIIJJJJJJJ)V");
		return env->NewObject(structStatClass, ctor, static_cast<jlong>(st.st_dev), static_cast<jlong>(st.st_ino), static_cast<jint>(st.st_mode),
		                      static_cast<jlong>(st.st_nlink), static_cast<jint>(st.st_uid), static_cast<jint>(st.st_gid), static_cast<jlong>(st.st_rdev),
		                      static_cast<jlong>(st.st_size), static_cast<jlong>(st.st_atime), static_cast<jlong>(st.st_mtime), static_cast<jlong>(st.st_ctime),
		                      static_cast<jlong>(st.st_blksize), static_cast<jlong>(st.st_blocks));
	}

	JNIEXPORT void JNICALL Java_libcore_io_Linux_close(JNIEnv* env, jclass clazz, jobject fdObj) {
		if (fdObj == nullptr) {
			return;
		}
		auto fdObject = sandvik::native::getObject(fdObj);
		int fd = fdObject->getField("descriptor")->getValue();
		if (fd >= 0 && ::close(fd) == -1) {
			throw IOException(fmt::format("close failed: {}", strerror(errno)));
		}
	}

	JNIEXPORT jstring JNICALL Java_libcore_io_Linux_strerror(JNIEnv* env, jclass clazz, jint error) {
		const char* message = ::strerror(error);
		if (message == nullptr) {
			return nullptr;
		}
		return env->NewStringUTF(message);
	}

	// fdsan owner-tag tracking is a debug aid (use-after-close/double-close detection); no-op here.
	JNIEXPORT void JNICALL Java_libcore_io_Linux_android_fdsan_exchange_owner_tag(JNIEnv*, jobject, jobject, jlong, jlong) {
	}
}  // extern "C"
