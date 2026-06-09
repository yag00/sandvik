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
#include "vm.hpp"

/** @todo implementation **/

extern "C" {

#if 0
JNIEXPORT void JNICALL Java_java_lang_Runtime_nativeExit(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Runtime_freeMemory(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Runtime_totalMemory(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Runtime_maxMemory(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Runtime_gc(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Runtime_nativeGc(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_java_lang_Runtime_runFinalization0(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

	JNIEXPORT jstring JNICALL Java_java_lang_Runtime_nativeLoad__Ljava_lang_String_2Ljava_lang_ClassLoader_2Ljava_lang_Class_2(JNIEnv* env, jclass,
	                                                                                                                           jstring filename, jobject,
	                                                                                                                           jclass) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto filenameObj = sandvik::native::getString(filename);
		const std::string& libName = filenameObj->str();
		try {
			jenv->getVm().loadLibrary(libName);
			return nullptr;  // success
		} catch (const std::exception& e) {
			logger.error(e.what());
			return env->NewStringUTF(e.what());
		}
	}

}  // extern "C"
