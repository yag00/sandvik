#include "test_arrays.h"

JNIEXPORT jintArray JNICALL Java_TestJNIArrays_nativeArraySum(JNIEnv* env, jobject thiz, jintArray arr) {
    jsize len = (*env)->GetArrayLength(env, arr);
    jint* data = (*env)->GetIntArrayElements(env, arr, NULL);
    jint sum = 0;
    for (int i = 0; i < len; i++) sum += data[i];
    (*env)->ReleaseIntArrayElements(env, arr, data, 0);

    jintArray out = (*env)->NewIntArray(env, 1);
    (*env)->SetIntArrayRegion(env, out, 0, 1, &sum);
    return out;
}