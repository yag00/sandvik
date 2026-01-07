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

#include <pwd.h>

#include <fmt/format.h>
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
		logger.fwarning("call sysconf(name={})", name);
		long result = 0;  // sysconf(name);
		if (result == -1 && errno != 0) {
			logger.ferror("sysconf failed with error: {}", strerror(errno));
			return -1;
		}
		return static_cast<jlong>(result);
	}
}  // extern "C"
