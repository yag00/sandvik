#include "test_helloworld.h"
#include <stdio.h>

JNIEXPORT jstring JNICALL Java_TestJNIHelloWorld_nativeHello(JNIEnv* env, jobject thiz, jstring name) {
    const char* n = (*env)->GetStringUTFChars(env, name, NULL);
    char buff[256];
    int size = (int)(*env)->GetStringUTFLength(env, name);
    snprintf(buff, sizeof(buff), "Hello %s %d", n, size);
    (*env)->ReleaseStringUTFChars(env, name, n);
    return (*env)->NewStringUTF(env, buff);
}
