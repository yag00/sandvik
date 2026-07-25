#include "test_refs.h"

static jobject globalRef = NULL;
static jweak weakRef = NULL;

JNIEXPORT jstring JNICALL Java_TestJNIRefs_nativeLocalRefTest(JNIEnv* env, jobject thiz) {
    for (int i = 0; i < 1000; i++) {
        jstring tmp = (*env)->NewStringUTF(env, "temp");
        (*env)->DeleteLocalRef(env, tmp);
    }
    return (*env)->NewStringUTF(env, "OK");
}

JNIEXPORT jstring JNICALL Java_TestJNIRefs_nativeGlobalRefTest(JNIEnv* env, jobject thiz) {
    if (!globalRef) {
        globalRef = (*env)->NewGlobalRef(env, thiz);
    }

    if (globalRef == NULL) {
        return (*env)->NewStringUTF(env, "FAIL");
    }

    return (*env)->NewStringUTF(env, "OK");
}

JNIEXPORT jstring JNICALL Java_TestJNIRefs_nativeWeakRefTest(JNIEnv* env, jobject thiz) {
    if (!weakRef) {
        weakRef = (*env)->NewWeakGlobalRef(env, thiz);
    }
    jobject strong = (*env)->NewLocalRef(env, weakRef);
    if (strong == NULL) {
        return (*env)->NewStringUTF(env, "FAIL");
    }
    (*env)->DeleteLocalRef(env, strong);
    return (*env)->NewStringUTF(env, "OK");
}