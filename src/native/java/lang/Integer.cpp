#include <fmt/format.h>
#include <jni/jni.h>

#include "class.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "jnihandlemap.hpp"
#include "object.hpp"
#include "system/logger.hpp"

extern "C" {
	JNIEXPORT jint JNICALL Java_java_lang_Integer_parseInt__Ljava_lang_String_2(JNIEnv* env, jclass clazz, jstring str) {
		const char* nativeStr = env->GetStringUTFChars(str, nullptr);
		if (nativeStr == nullptr) {
			// @todo throw a Java exception here
			logger.warning("@todo throw a Java exception here");
			return 0;
		}
		int value = 0;
		try {
			value = std::stoi(nativeStr);
		} catch (const std::exception&) {
			// @todo throw a Java exception here
			logger.warning("@todo throw a Java exception here");
			value = 0;
		}
		env->ReleaseStringUTFChars(str, nativeStr);
		return static_cast<jint>(value);
	}
}