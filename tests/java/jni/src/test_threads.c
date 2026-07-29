#include "test_threads.h"

static void* thread_fn(void* arg) {
    JavaVM* jvm = (JavaVM*)arg;
    JNIEnv* env;

    if ((*jvm)->AttachCurrentThread(jvm, (void **)&env, NULL) != 0)
        return NULL;

    (*jvm)->DetachCurrentThread(jvm);
    return NULL;
}

JNIEXPORT jboolean JNICALL Java_TestJNIThreads_nativeThreadTest(JNIEnv* env, jobject thiz) {
    JavaVM* jvm;
    (*env)->GetJavaVM(env, &jvm);

    pthread_t t;
    pthread_create(&t, NULL, thread_fn, jvm);
    pthread_join(t, NULL);

    return JNI_TRUE;
}