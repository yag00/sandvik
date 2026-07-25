#ifndef TEST_ARRAYS_H
#define TEST_ARRAYS_H

#include "jni_common.h"

JNIEXPORT jintArray JNICALL Java_TestJNIArrays_nativeArraySum(JNIEnv* env, jobject thiz, jintArray arr);

#endif