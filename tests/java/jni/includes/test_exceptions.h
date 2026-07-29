#ifndef TEST_EXCEPTIONS_H
#define TEST_EXCEPTIONS_H

#include "jni_common.h"

JNIEXPORT jboolean JNICALL Java_TestJNIExceptions_nativeThrowAndCatch(JNIEnv* env, jobject thiz);

#endif