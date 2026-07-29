#ifndef TEST_EXCEPTIONS_H
#define TEST_EXCEPTIONS_H

#include "jni_common.h"

JNIEXPORT jboolean JNICALL Java_TestJNI_nativeIsInstanceOf(JNIEnv* env, jobject thiz, jobject obj, jstring className);

#endif