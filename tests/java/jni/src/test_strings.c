#include "test_strings.h"
#include <stdio.h>

JNIEXPORT jstring JNICALL Java_TestJNIStrings_nativeUnicodeStringTest(JNIEnv* env, jobject thiz, jstring input) {
    jsize len = (*env)->GetStringLength(env, input);
    jboolean isCopy;
    const jchar* chars = (*env)->GetStringChars(env, input, &isCopy);
    jstring out = (*env)->NewString(env, chars, len);
    (*env)->ReleaseStringChars(env, input, chars);
    return out;
}