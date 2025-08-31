#include <fmt/format.h>
#include <jni/jni.h>

#include "class.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "jnihandlemap.hpp"
#include "object.hpp"
#include "system/logger.hpp"

extern "C" {
	JNIEXPORT jboolean JNICALL Java_java_lang_Double_isNaN__D(JNIEnv *, jclass, jdouble value) {
		return std::isnan(value) ? JNI_TRUE : JNI_FALSE;
	}
}