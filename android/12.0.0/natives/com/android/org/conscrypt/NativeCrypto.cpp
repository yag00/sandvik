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
#include <openssl/err.h>
#include <openssl/ssl.h>

#include "array.hpp"
#include "classloader.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "system/logger.hpp"

using namespace sandvik;

extern "C" {
	JNIEXPORT void JNICALL Java_com_android_org_conscrypt_NativeCrypto_clinit(JNIEnv* env, jclass clazz) {
		logger.fdebug("NativeCrypto.clinit()");
	}

	JNIEXPORT jobjectArray JNICALL Java_com_android_org_conscrypt_NativeCrypto_get_cipher_names(JNIEnv* env, jclass clazz, jstring selector) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto selectorObj = sandvik::native::getObject(selector);

		logger.fdebug("[NativeCrypto.get_cipher_names] selector={}", selectorObj ? selectorObj->toString() : "null");

		// TODO: Improve this list
		std::vector<std::string> ciphers = {"ECDHE-ECDSA-AES128-GCM-SHA256",
		                                    "ECDHE-ECDSA-AES256-GCM-SHA384",
		                                    "ECDHE-ECDSA-CHACHA20-POLY1305",

		                                    "ECDHE-RSA-AES128-GCM-SHA256",
		                                    "ECDHE-RSA-AES256-GCM-SHA384",
		                                    "ECDHE-RSA-CHACHA20-POLY1305",

		                                    "ECDHE-ECDSA-AES128-SHA",
		                                    "ECDHE-ECDSA-AES256-SHA",

		                                    "ECDHE-RSA-AES128-SHA",
		                                    "ECDHE-RSA-AES256-SHA",

		                                    "AES128-GCM-SHA256",
		                                    "AES256-GCM-SHA384",

		                                    "AES128-SHA",
		                                    "AES256-SHA"};

		auto& classloader = jenv->getClassLoader();

		auto& stringClass = classloader.getOrLoad("java.lang.String");
		auto result = Array::make(stringClass, ciphers.size());

		for (size_t i = 0; i < ciphers.size(); ++i) {
			auto value = sandvik::Object::make(classloader, ciphers[i]);
			result->setElement(static_cast<uint32_t>(i), value);
		}

		auto selectorString = selectorObj ? selectorObj->toString() : "";
		// TODO: Only supports "ALL:!DHE" case for now
		if (selectorString != "ALL:!DHE") {
			logger.fwarning("[NativeCrypto.get_cipher_names] unsupported selector={}", selectorString);
		}

		return (jobjectArray)result;
	}

	JNIEXPORT jboolean JNICALL Java_com_android_org_conscrypt_NativeCrypto_EVP_has_aes_hardware(JNIEnv* env, jclass clazz) {
		logger.fdebug("[NativeCrypto.EVP_has_aes_hardware]");

		return JNI_FALSE;
	}
}  // extern "C"