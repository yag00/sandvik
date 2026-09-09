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

#include "exceptions.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"

using namespace sandvik;

extern "C" {
	JNIEXPORT jobject JNICALL Java_java_lang_ref_FinalizerReference_getReferent(JNIEnv* env, jobject thiz) {
		if (thiz == nullptr) {
			throw NullPointerException("getReferent called on null object");
		}
		auto obj = sandvik::native::getObject(thiz);
		auto referent = obj->getField("referent");
		return (jobject)referent;
	}

	JNIEXPORT void JNICALL Java_java_lang_ref_Reference_clearReferent(JNIEnv* env, jobject thiz) {
		if (thiz == nullptr) {
			throw NullPointerException("clearReferent called on null object");
		}
		auto obj = sandvik::native::getObject(thiz);
		obj->setField("referent", Object::makeNull());
	}

	JNIEXPORT jboolean JNICALL Java_java_lang_ref_FinalizerReference_makeCircularListIfUnenqueued(JNIEnv* env, jobject thiz) {
		if (thiz == nullptr) {
			throw NullPointerException("makeCircularListIfUnenqueued called on null object");
		}
		auto obj = sandvik::native::getObject(thiz);

		// pendingNext is a field in the Reference class that is used to create a circular linked list of references that are pending finalization. If
		// pendingNext is not null, it means that the reference has already been enqueued for finalization, and we should not modify the list. If pendingNext is
		// null, we set it to point to itself, creating a circular list with one element.
		auto pendingNext = obj->getField("pendingNext");
		if (!pendingNext->isNull()) {
			// If pendingNext is not null, it means the reference has already been enqueued for finalization, so we return JNI_FALSE to indicate that we did not
			// create a circular list.
			return JNI_FALSE;
		}
		obj->setField("pendingNext", obj);  // circular list with one element: points to itself
		return JNI_TRUE;
	}
}  // extern "C"