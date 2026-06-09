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

extern "C" {

#if 0
JNIEXPORT void JNICALL Java_java_lang_System_setIn0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_System_setOut0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_System_setErr0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

	JNIEXPORT jlong JNICALL Java_java_lang_System_currentTimeMillis(JNIEnv* env, jobject obj) {
		struct timeval now;
		gettimeofday(&now, NULL);
		return now.tv_sec * 1000LL + now.tv_usec / 1000;
	}

	JNIEXPORT jlong JNICALL Java_java_lang_System_nanoTime(JNIEnv* env, jobject obj) {
		struct timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		return now.tv_sec * 1000000000LL + now.tv_nsec;
	}

#if 0
JNIEXPORT void JNICALL Java_java_lang_System_arraycopy(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_System_arraycopyCharUnchecked(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_System_arraycopyByteUnchecked(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_System_arraycopyShortUnchecked(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_System_arraycopyIntUnchecked(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_System_arraycopyLongUnchecked(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_System_arraycopyFloatUnchecked(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_System_arraycopyDoubleUnchecked(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_System_arraycopyBooleanUnchecked(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

	JNIEXPORT void JNICALL Java_java_lang_System_arraycopy__Ljava_lang_Object_2ILjava_lang_Object_2II(JNIEnv* env, jclass, jobject src, jint srcPos,
	                                                                                                  jobject dest, jint destPos, jint length) {
		sandvik::ArrayRef srcArray = sandvik::native::getArray(src);
		sandvik::ArrayRef destArray = sandvik::native::getArray(dest);

		jint srcLen = srcArray->getArrayLength();
		jint destLen = destArray->getArrayLength();

		if (srcPos < 0 || destPos < 0 || length < 0 || srcPos + length > srcLen || destPos + length > destLen) {
			throw sandvik::IndexOutOfBoundsException("Invalid arraycopy range");
		}

		// Copy elements
		for (jint i = 0; i < length; i++) {
			sandvik::ObjectRef element = srcArray->getElement(srcPos + i);
			destArray->setElement(destPos + i, element);
		}
	}

	JNIEXPORT jobjectArray JNICALL Java_java_lang_System_specialProperties(JNIEnv* env, jclass clazz) {
		jclass stringClass = env->FindClass("java/lang/String");
		jobjectArray result = env->NewObjectArray(4, stringClass, NULL);

		char path[PATH_MAX];
		char* process_path = getcwd(path, sizeof(path));
		std::string user_dir_s = "user.dir=";
		if (process_path != nullptr) {
			user_dir_s += process_path;
		}
		const char* user_dir = user_dir_s.c_str();
		jstring user_dir_str = env->NewStringUTF(user_dir);
		env->SetObjectArrayElement(result, 0, user_dir_str);
		jstring zlib_str = env->NewStringUTF("android.zlib.version=1.2.11");
		env->SetObjectArrayElement(result, 1, zlib_str);
		jstring ssl_str = env->NewStringUTF("android.openssl.version=OpenSSL 3.0.2 15 Mar 2022");
		env->SetObjectArrayElement(result, 2, ssl_str);
		const char* library_path = getenv("LD_LIBRARY_PATH");
		if (library_path == NULL) {
			library_path = "";
		}
		std::string java_path = "java.library.path=";
		java_path += library_path;
		jstring java_path_str = env->NewStringUTF(java_path.c_str());
		env->SetObjectArrayElement(result, 3, java_path_str);
		return result;
	}

	JNIEXPORT jstring JNICALL Java_java_lang_System_mapLibraryName(JNIEnv* env, jclass, jstring libname) {
		auto nameObj = sandvik::native::getString(libname);
		std::string mapped = "lib" + nameObj->str() + ".so";
		return env->NewStringUTF(mapped.c_str());
	}

	JNIEXPORT void JNICALL Java_java_lang_System_log(JNIEnv* env, jclass ignored, jchar type, jstring javaMessage, jthrowable exception) {
		std::string priority;
		switch (type) {
			case 'D':
			case 'd':
				priority = "ANDROID_LOG_DEBUG";
				break;
			case 'E':
			case 'e':
				priority = "ANDROID_LOG_ERROR";
				break;
			case 'F':
			case 'f':
				priority = "ANDROID_LOG_FATAL";
				break;
			case 'I':
			case 'i':
				priority = "ANDROID_LOG_INFO";
				break;
			case 'S':
			case 's':
				priority = "ANDROID_LOG_SILENT";
				break;
			case 'V':
			case 'v':
				priority = "ANDROID_LOG_VERBOSE";
				break;
			case 'W':
			case 'w':
				priority = "ANDROID_LOG_WARN";
				break;
			default:
				priority = "ANDROID_LOG_DEFAULT";
				break;
		}

		logger.finfo("[{}] System {}", priority, sandvik::native::getString(javaMessage)->str());

		/*if (exception != NULL) {
		    logger.finfo("[{}] System Exception {}", priority, sandvik::native::getObject(exception)->getClass().getFullname());
		}*/
	}

}  // extern "C"
