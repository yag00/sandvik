#include <fmt/format.h>
#include <jni/jni.h>

#include "class.hpp"
#include "classloader.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "jnihandlemap.hpp"
#include "object.hpp"
#include "system/logger.hpp"

extern "C" {
	JNIEXPORT void JNICALL Java_java_lang_String_initialize__Ljava_lang_String_2(JNIEnv* env, jobject obj, jobject other) {
		// Check if obj or other is null
		sandvik::Object* this_ptr = (sandvik::Object*)obj;
		sandvik::Object* other_ptr = (sandvik::Object*)other;
		if (this_ptr == nullptr || other_ptr == nullptr) {
			throw std::runtime_error("NullPointerException");
		}
		if (!this_ptr->isString()) {
			throw std::runtime_error("Object is not a java.lang.String");
		}
		if (!other_ptr->isString()) {
			throw std::runtime_error("Object is not a java.lang.String");
		}
		this_ptr->setString(this_ptr->str() + other_ptr->str());
	}

	JNIEXPORT jboolean JNICALL Java_java_lang_String_equals(JNIEnv* env, jobject obj, jobject other) {
		// Check if the current object (this) is null
		sandvik::Object* this_ptr = (sandvik::Object*)obj;
		if (this_ptr == nullptr) {
			throw std::runtime_error("NullPointerException");
		}
		// If the other object is null, return false
		if (other == nullptr) {
			return JNI_FALSE;
		}
		// Cast the other object to sandvik::Object
		sandvik::Object* other_ptr = (sandvik::Object*)other;
		// If either string field is null, return false
		if (this_ptr->isNull() || other_ptr->isNull()) {
			return JNI_FALSE;
		}
		if (!this_ptr->isString() || !other_ptr->isString()) {
			return JNI_FALSE;
		}
		// Get the actual string values
		auto this_str = this_ptr->str();
		auto other_str = other_ptr->str();
		// Compare the strings for equality
		return this_str == other_str ? JNI_TRUE : JNI_FALSE;
	}
}