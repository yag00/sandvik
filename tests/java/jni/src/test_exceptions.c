#include "test_exceptions.h"

JNIEXPORT jboolean JNICALL Java_TestJNIExceptions_nativeThrowAndCatch(JNIEnv* env, jobject thiz) {
    jclass exc = (*env)->FindClass(env, "java/lang/RuntimeException");
    (*env)->ThrowNew(env, exc, "Test exception");

    if (!(*env)->ExceptionCheck(env)) return JNI_FALSE;

    (*env)->ExceptionClear(env);
    return JNI_TRUE;
}
