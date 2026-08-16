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

#include "array.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

extern "C" {

	JNIEXPORT jobject JNICALL Java_com_android_internal_os_ClassLoaderFactory_getClassLoader(JNIEnv* env, jclass, jstring classLoaderName) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto& classloader = jenv->getClassLoader();

		auto nameObj = sandvik::native::getString(classLoaderName);
		if (!nameObj) {
			return nullptr;
		}

		auto& loaderClass = classloader.getOrLoad("java/lang/ClassLoader");
		jclass jloaderClass = (jclass)Object::make(loaderClass);

		jmethodID ctorID = env->GetMethodID(jloaderClass, "<init>", "(Ljava/lang/String;)V");
		if (!ctorID) {
			throw NoSuchMethodException("ClassLoader constructor (Ljava/lang/String;)V not found");
		}

		return env->NewObject(jloaderClass, ctorID, (jobject)Object::make(classloader, nameObj->str()));
	}

	JNIEXPORT jstring JNICALL Java_com_android_internal_os_ClassLoaderFactory_createClassloaderNamespace(JNIEnv* env, jclass, jobject classLoader,
	                                                                                                     jint targetSdkVersion, jstring librarySearchPath,
	                                                                                                     jstring libraryPermittedPath,
	                                                                                                     jboolean isNamespaceShared, jstring dexPath,
	                                                                                                     jstring permittedPath) {
		(void)env;
		(void)classLoader;
		(void)targetSdkVersion;
		(void)isNamespaceShared;

		// Ces paramètres sont souvent null en pratique (namespace par défaut,
		// pas de chemins de recherche natifs spécifiques) : ne pas les caster
		// sans vérifier isNull() d'abord.
		std::string searchPath;
		if (librarySearchPath != nullptr) {
			auto searchPathObj = sandvik::native::getString(librarySearchPath);
			if (searchPathObj) {
				searchPath = searchPathObj->str();
			}
		}

		std::string permittedLibPath;
		if (libraryPermittedPath != nullptr) {
			auto permittedLibObj = sandvik::native::getString(libraryPermittedPath);
			if (permittedLibObj) {
				permittedLibPath = permittedLibObj->str();
			}
		}

		std::string dexPathStr;
		if (dexPath != nullptr) {
			auto dexPathObj = sandvik::native::getString(dexPath);
			if (dexPathObj) {
				dexPathStr = dexPathObj->str();
			}
		}

		std::string permittedPathStr;
		if (permittedPath != nullptr) {
			auto permittedPathObj = sandvik::native::getString(permittedPath);
			if (permittedPathObj) {
				permittedPathStr = permittedPathObj->str();
			}
		}

		logger.fdebug("createClassloaderNamespace: searchPath='{}', permittedLibPath='{}', dexPath='{}', permittedPath='{}'", searchPath, permittedLibPath,
		              dexPathStr, permittedPathStr);

		// Sandvik n'implémente pas de véritable namespace linker natif (pas de
		// chargement de bibliothèques .so séparées par classloader) : on
		// retourne null pour signaler le succès, comme le fait AOSP quand la
		// création du namespace réussit (un message d'erreur String n'est
		// retourné qu'en cas d'échec).
		return nullptr;
	}
}