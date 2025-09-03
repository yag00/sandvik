#include <fmt/format.h>
#include <jni/jni.h>

#include "class.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "jnihandlemap.hpp"
#include "object.hpp"
#include "system/logger.hpp"

extern "C" {
	JNIEXPORT jint JNICALL Java_java_lang_Throwable_fillInStackTrace__I(JNIEnv* env, jobject obj) {
		sandvik::NativeInterface* jenv = static_cast<sandvik::NativeInterface*>(env);
		if (jenv == nullptr) {
			throw std::runtime_error("Internal error: JNIEnv is not a NativeInterface");
		}
		sandvik::Object* ptr = (sandvik::Object*)obj;
		if (ptr == nullptr) {
			throw std::runtime_error("NullPointerException");
		}
		//@todo capture the stack trace and store it in the Throwable object.
		// For now, just return 0 to indicate success.
		return 0;
	}
}