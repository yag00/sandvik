#include "test_objects.h"

JNIEXPORT jobject JNICALL Java_TestJNIObjects_nativeNewObject(JNIEnv* env, jobject thiz, jstring message, jint i) {
    jclass cls = (*env)->FindClass(env, "NativeTestObject");
    if (cls == NULL) return NULL;
    jmethodID ctor = (*env)->GetMethodID(env, cls, "<init>", "(Ljava/lang/String;I)V");
    jobject obj = (*env)->NewObject(env, cls, ctor, message, i);
    if (obj == NULL) {
        (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/InstantiationException"), "Failed to create NativeTestObject");
    }
    (*env)->DeleteLocalRef(env, cls);
    return obj;
}

JNIEXPORT jobject JNICALL Java_TestJNIObjects_nativeNewObject2(JNIEnv* env, jobject thiz, jstring message, jint i, jdouble d, jfloat f, jlong l, jshort s, jbyte b, jchar c, jboolean bool_) {
    jclass cls = (*env)->FindClass(env, "NativeTestObject");
    if (cls == NULL) return NULL;
    jmethodID ctor = (*env)->GetMethodID(env, cls, "<init>", "(Ljava/lang/String;IDFJSBCZ)V");
    jobject obj = (*env)->NewObject(env, cls, ctor, message, i, d, f, l, s, b, c, bool_);
    if (obj == NULL) {
        (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/InstantiationException"), "Failed to create NativeTestObject");
    }
    (*env)->DeleteLocalRef(env, cls);
    return obj;
}

JNIEXPORT jobject JNICALL Java_TestJNIObjects_nativeNewObjectV(JNIEnv* env, jobject thiz, ...) {
    jclass cls = (*env)->FindClass(env, "NativeTestObject");
    if (cls == NULL) {
        (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/ClassNotFoundException"), "NativeTestObject not found");
        return NULL;
    }
    jmethodID ctor = (*env)->GetMethodID(env, cls, "<init>", "(Ljava/lang/String;IDFJSBCZ)V");
    va_list args;
    va_start(args, thiz);
    jobject obj = (*env)->NewObjectV(env, cls, ctor, args);
    va_end(args);
    (*env)->DeleteLocalRef(env, cls);
    if (obj == NULL) {
        (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/InstantiationException"), "Failed to create NativeTestObject");
    }
    return obj;
}

JNIEXPORT jobject JNICALL Java_TestJNIObjects_nativeNewObjectA(JNIEnv* env, jobject thiz, jobjectArray args) {
    jclass cls = (*env)->FindClass(env, "NativeTestObject");
    if (cls == NULL) {
        (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/ClassNotFoundException"), "NativeTestObject not found");
        return NULL;
    }
    jmethodID ctor = (*env)->GetMethodID(env, cls, "<init>", "(Ljava/lang/String;IDFJSBCZ)V");
    jsize len = (*env)->GetArrayLength(env, args);
    jvalue* jargs = (jvalue*)malloc(len * sizeof(jvalue));
    if (jargs == NULL) {
        (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/OutOfMemoryError"), "Failed to allocate memory for jvalue array");
        (*env)->DeleteLocalRef(env, cls);
        return NULL;
    }

    jargs[0].l = (*env)->GetObjectArrayElement(env, args, 0);
    jargs[1].i = (*env)->CallIntMethod(env, (*env)->GetObjectArrayElement(env, args, 1), (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Integer"), "intValue", "()I"));
    jargs[2].d = (*env)->CallDoubleMethod(env, (*env)->GetObjectArrayElement(env, args, 2), (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Double"), "doubleValue", "()D"));
    jargs[3].f = (*env)->CallFloatMethod(env, (*env)->GetObjectArrayElement(env, args, 3), (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Float"), "floatValue", "()F"));
    jargs[4].j = (*env)->CallLongMethod(env, (*env)->GetObjectArrayElement(env, args, 4), (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Long"), "longValue", "()J"));
    jargs[5].s = (*env)->CallShortMethod(env, (*env)->GetObjectArrayElement(env, args, 5), (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Short"), "shortValue", "()S"));
    jargs[6].b = (*env)->CallByteMethod(env, (*env)->GetObjectArrayElement(env, args, 6), (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Byte"), "byteValue", "()B"));
    jargs[7].c = (*env)->CallCharMethod(env, (*env)->GetObjectArrayElement(env, args, 7), (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Character"), "charValue", "()C"));
    jargs[8].z = (*env)->CallBooleanMethod(env, (*env)->GetObjectArrayElement(env, args, 8), (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Boolean"), "booleanValue", "()Z"));

    jobject obj = (*env)->NewObjectA(env, cls, ctor, jargs);
    free(jargs);
    (*env)->DeleteLocalRef(env, cls);
    if (obj == NULL) {
        (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/InstantiationException"), "Failed to create NativeTestObject");
    }
    return obj;
}