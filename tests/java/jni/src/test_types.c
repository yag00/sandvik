#include "test_types.h"

JNIEXPORT jboolean JNICALL Java_TestJNITypes_nativeIsInstanceOf(JNIEnv* env, jobject thiz, jobject obj, jstring className) {
    const char* cname = (*env)->GetStringUTFChars(env, className, NULL);
    jclass cls = (*env)->FindClass(env, cname);
    (*env)->ReleaseStringUTFChars(env, className, cname);
    if (cls == NULL) {
        return JNI_FALSE;
    }
    jboolean result = (*env)->IsInstanceOf(env, obj, cls);
    (*env)->DeleteLocalRef(env, cls);
    return result;
}