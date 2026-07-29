#ifndef TEST_STRINGS_H
#define TEST_STRINGS_H

#include "jni_common.h"

JNIEXPORT jstring JNICALL Java_TestJNIStrings_nativeUnicodeStringTest(JNIEnv* env, jobject thiz, jstring input);

#endif