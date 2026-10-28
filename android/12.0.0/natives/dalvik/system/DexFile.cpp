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

#include <string>

#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "vfs.hpp"

/*
 * Sandvik has a single class namespace: every dex (runtime jars, --dex, --apk, and the files opened here) is loaded
 * into the VM ClassLoader. A DexFile cookie is the host path of the opened file; defineClassNative looks the class up
 * in the VM ClassLoader, so app classes report the boot class loader as their loader.
 */

using namespace sandvik;

namespace {
	std::string toString(JNIEnv* env_, jstring str_) {
		if (str_ == nullptr) {
			return "";
		}
		const char* utf8 = env_->GetStringUTFChars(str_, nullptr);
		std::string result(utf8);
		env_->ReleaseStringUTFChars(str_, utf8);
		return result;
	}
}  // namespace

extern "C" {

	JNIEXPORT jobject JNICALL Java_dalvik_system_DexFile_openDexFileNative(JNIEnv* env, jclass, jstring sourceName, jstring, jint, jobject, jobjectArray) {
		const auto path = toString(env, sourceName);
		const auto hostPath = VFS::resolve(path);
		struct stat st;
		if (stat(hostPath.c_str(), &st) != 0) {
			throw IOException("No original dex files found for dex location " + path);
		}
		auto& classloader = native::getNativeInterface(env)->getClassLoader();
		if (hostPath.ends_with(".dex")) {
			classloader.loadDex(hostPath);
		} else if (!classloader.isApkLoaded(hostPath)) {
			// .apk and .jar both are zip files with classes*.dex
			classloader.loadApk(hostPath);
		}
		logger.fdebug("DexFile.openDexFileNative: {} ({})", path, hostPath);
		return env->NewStringUTF(hostPath.c_str());
	}

	JNIEXPORT jobject JNICALL Java_dalvik_system_DexFile_openInMemoryDexFilesNative(JNIEnv*, jclass, jobjectArray, jobjectArray, jintArray, jintArray, jobject,
	                                                                                jobjectArray) {
		throw IOException("In memory dex files are not supported");
	}

	JNIEXPORT jclass JNICALL Java_dalvik_system_DexFile_defineClassNative(JNIEnv* env, jclass, jstring name, jobject, jobject, jobject) {
		auto& classloader = native::getNativeInterface(env)->getClassLoader();
		try {
			auto& clazz = classloader.getOrLoad(toString(env, name));
			return (jclass)Object::makeConstClass(classloader, clazz);
		} catch (const std::exception&) {
			// not found: the caller throws ClassNotFoundException
			return nullptr;
		}
	}

	JNIEXPORT jobjectArray JNICALL Java_dalvik_system_DexFile_getClassNameList(JNIEnv* env, jclass, jobject) {
		logger.fwarning("DexFile.getClassNameList not implemented");
		return env->NewObjectArray(0, env->FindClass("java/lang/String"), nullptr);
	}

	JNIEXPORT jboolean JNICALL Java_dalvik_system_DexFile_closeDexFile(JNIEnv*, jclass, jobject) {
		return JNI_TRUE;
	}

	JNIEXPORT jboolean JNICALL Java_dalvik_system_DexFile_isBackedByOatFile__Ljava_lang_Object_2(JNIEnv*, jclass, jobject) {
		return JNI_FALSE;
	}

	JNIEXPORT jlong JNICALL Java_dalvik_system_DexFile_getStaticSizeOfDexFile__Ljava_lang_Object_2(JNIEnv* env, jclass, jobject cookie) {
		struct stat st;
		return stat(toString(env, (jstring)cookie).c_str(), &st) == 0 ? static_cast<jlong>(st.st_size) : 0;
	}

	JNIEXPORT void JNICALL Java_dalvik_system_DexFile_setTrusted__Ljava_lang_Object_2(JNIEnv*, jclass, jobject) {
	}

	JNIEXPORT void JNICALL Java_dalvik_system_DexFile_verifyInBackgroundNative(JNIEnv*, jclass, jobject, jobject) {
	}

	// No dex2oat: the code always runs from the dex files

	JNIEXPORT jint JNICALL Java_dalvik_system_DexFile_getDexOptNeeded__Ljava_lang_String_2Ljava_lang_String_2Ljava_lang_String_2Ljava_lang_String_2ZZ(
	    JNIEnv*, jclass, jstring, jstring, jstring, jstring, jboolean, jboolean) {
		// DexFile.NO_DEXOPT_NEEDED
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_dalvik_system_DexFile_isDexOptNeeded(JNIEnv*, jclass, jstring) {
		return JNI_FALSE;
	}

	JNIEXPORT jstring JNICALL Java_dalvik_system_DexFile_getDexFileStatus(JNIEnv* env, jclass, jstring, jstring) {
		return env->NewStringUTF("run-from-apk");
	}

	JNIEXPORT jobjectArray JNICALL Java_dalvik_system_DexFile_getDexFileOutputPaths(JNIEnv*, jclass, jstring, jstring) {
		return nullptr;
	}

	JNIEXPORT jobjectArray JNICALL Java_dalvik_system_DexFile_getDexFileOptimizationStatus(JNIEnv* env, jclass, jstring, jstring) {
		jobjectArray result = env->NewObjectArray(2, env->FindClass("java/lang/String"), nullptr);
		env->SetObjectArrayElement(result, 0, env->NewStringUTF("run-from-apk"));
		env->SetObjectArrayElement(result, 1, env->NewStringUTF("unknown"));
		return result;
	}

	JNIEXPORT jboolean JNICALL Java_dalvik_system_DexFile_isValidCompilerFilter(JNIEnv*, jclass, jstring) {
		return JNI_TRUE;
	}

	JNIEXPORT jboolean JNICALL Java_dalvik_system_DexFile_isProfileGuidedCompilerFilter(JNIEnv*, jclass, jstring) {
		return JNI_FALSE;
	}

	JNIEXPORT jstring JNICALL Java_dalvik_system_DexFile_getNonProfileGuidedCompilerFilter(JNIEnv*, jclass, jstring filter) {
		return filter;
	}

	JNIEXPORT jstring JNICALL Java_dalvik_system_DexFile_getSafeModeCompilerFilter(JNIEnv*, jclass, jstring filter) {
		return filter;
	}

}  // extern "C"
