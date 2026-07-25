#ifndef TEST_REFS_H
#define TEST_REFS_H

#include "jni_common.h"

JNIEXPORT jstring JNICALL Java_TestJNIRefs_nativeLocalRefTest(JNIEnv* env, jobject thiz);
JNIEXPORT jstring JNICALL Java_TestJNIRefs_nativeGlobalRefTest(JNIEnv* env, jobject thiz);
JNIEXPORT jstring JNICALL Java_TestJNIRefs_nativeWeakRefTest(JNIEnv* env, jobject thiz);

#endif