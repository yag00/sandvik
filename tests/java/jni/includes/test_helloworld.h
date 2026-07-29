#ifndef TEST_HELLOWORLD_H
#define TEST_HELLOWORLD_H

#include "jni_common.h"

JNIEXPORT jstring JNICALL Java_TestJNIHelloWorld_nativeHello(JNIEnv* env, jobject thiz, jstring name);

#endif