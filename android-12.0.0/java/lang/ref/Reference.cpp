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

using namespace sandvik;

extern "C" {

#if 0
JNIEXPORT void JNICALL Java_java_lang_ref_Reference_getReferent(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

	JNIEXPORT jboolean JNICALL Java_java_lang_ref_Reference_refersTo0(JNIEnv* env, jobject refObj, jobject obj) {
		try {
			auto ref = sandvik::native::getObject(refObj);
			auto target = sandvik::native::getObject(obj);
			// retrieve the referent field
			auto referent = ref->getField("referent");
			// compares object identity (==)
			return referent == target ? JNI_TRUE : JNI_FALSE;
		} catch (...) {
			return JNI_FALSE;
		}
	}

#if 0
JNIEXPORT void JNICALL Java_java_lang_ref_Reference_clearReferent(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

}  // extern "C"
