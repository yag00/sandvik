#ifndef TEST_FIELDS_H
#define TEST_FIELDS_H

#include "jni_common.h"

JNIEXPORT jint JNICALL Java_TestJNIFields_nativeGetIntField(JNIEnv* env, jobject thiz, jobject obj);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetIntField(JNIEnv* env, jobject thiz, jobject obj, jint v);
JNIEXPORT jboolean JNICALL Java_TestJNIFields_nativeGetBooleanField(JNIEnv* env, jobject thiz, jobject obj);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetBooleanField(JNIEnv* env, jobject thiz, jobject obj, jboolean v);
JNIEXPORT jbyte JNICALL Java_TestJNIFields_nativeGetByteField(JNIEnv* env, jobject thiz, jobject obj);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetByteField(JNIEnv* env, jobject thiz, jobject obj, jbyte v);
JNIEXPORT jchar JNICALL Java_TestJNIFields_nativeGetCharField(JNIEnv* env, jobject thiz, jobject obj);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetCharField(JNIEnv* env, jobject thiz, jobject obj, jchar v);
JNIEXPORT jshort JNICALL Java_TestJNIFields_nativeGetShortField(JNIEnv* env, jobject thiz, jobject obj);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetShortField(JNIEnv* env, jobject thiz, jobject obj, jshort v);
JNIEXPORT jlong JNICALL Java_TestJNIFields_nativeGetLongField(JNIEnv* env, jobject thiz, jobject obj);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetLongField(JNIEnv* env, jobject thiz, jobject obj, jlong v);
JNIEXPORT jfloat JNICALL Java_TestJNIFields_nativeGetFloatField(JNIEnv* env, jobject thiz, jobject obj);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetFloatField(JNIEnv* env, jobject thiz, jobject obj, jfloat v);
JNIEXPORT jdouble JNICALL Java_TestJNIFields_nativeGetDoubleField(JNIEnv* env, jobject thiz, jobject obj);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetDoubleField(JNIEnv* env, jobject thiz, jobject obj, jdouble v);
JNIEXPORT jobject JNICALL Java_TestJNIFields_nativeGetStringField(JNIEnv* env, jobject thiz, jobject obj);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStringField(JNIEnv* env, jobject thiz, jobject obj, jobject v);
JNIEXPORT jintArray JNICALL Java_TestJNIFields_nativeGetIntArrayField(JNIEnv* env, jobject thiz, jobject obj);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetIntArrayField(JNIEnv* env, jobject thiz, jobject obj, jintArray v);

JNIEXPORT jint JNICALL Java_TestJNIFields_nativeGetStaticIntField(JNIEnv* env, jclass clazz);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticIntField(JNIEnv* env, jclass clazz, jint v);
JNIEXPORT jdouble JNICALL Java_TestJNIFields_nativeGetStaticDoubleField(JNIEnv* env, jclass clazz);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticDoubleField(JNIEnv* env, jclass clazz, jdouble v);
JNIEXPORT jfloat JNICALL Java_TestJNIFields_nativeGetStaticFloatField(JNIEnv* env, jclass clazz);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticFloatField(JNIEnv* env, jclass clazz, jfloat v);
JNIEXPORT jlong JNICALL Java_TestJNIFields_nativeGetStaticLongField(JNIEnv* env, jclass clazz);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticLongField(JNIEnv* env, jclass clazz, jlong v);
JNIEXPORT jshort JNICALL Java_TestJNIFields_nativeGetStaticShortField(JNIEnv* env, jclass clazz);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticShortField(JNIEnv* env, jclass clazz, jshort v);
JNIEXPORT jbyte JNICALL Java_TestJNIFields_nativeGetStaticByteField(JNIEnv* env, jclass clazz);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticByteField(JNIEnv* env, jclass clazz, jbyte v);
JNIEXPORT jchar JNICALL Java_TestJNIFields_nativeGetStaticCharField(JNIEnv* env, jclass clazz);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticCharField(JNIEnv* env, jclass clazz, jchar v);
JNIEXPORT jboolean JNICALL Java_TestJNIFields_nativeGetStaticBooleanField(JNIEnv* env, jclass clazz);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticBooleanField(JNIEnv* env, jclass clazz, jboolean v);
JNIEXPORT jstring JNICALL Java_TestJNIFields_nativeGetStaticStringField(JNIEnv* env, jclass clazz);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticStringField(JNIEnv* env, jclass clazz, jstring v);
JNIEXPORT jintArray JNICALL Java_TestJNIFields_nativeGetStaticIntArrayField(JNIEnv* env, jclass clazz);
JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticIntArrayField(JNIEnv* env, jclass clazz, jintArray v);

#endif