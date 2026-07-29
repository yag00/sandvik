#ifndef TEST_OBJECTS_H
#define TEST_OBJECTS_H

#include "jni_common.h"

JNIEXPORT jobject JNICALL Java_TestJNIObjects_nativeNewObject(JNIEnv* env, jobject thiz, jstring message, jint i);
JNIEXPORT jobject JNICALL Java_TestJNIObjects_nativeNewObject2(JNIEnv* env, jobject thiz, jstring message, jint i, jdouble d, jfloat f, jlong l, jshort s, jbyte b, jchar c, jboolean bool_);
JNIEXPORT jobject JNICALL Java_TestJNIObjects_nativeNewObjectV(JNIEnv* env, jobject thiz, ...);
JNIEXPORT jobject JNICALL Java_TestJNIObjects_nativeNewObjectA(JNIEnv* env, jobject thiz, jobjectArray args);

#endif