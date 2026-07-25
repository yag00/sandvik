#ifndef TEST_METHODS_H
#define TEST_METHODS_H

#include "jni_common.h"

JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallObjectMethod(JNIEnv* env, jobject thiz, jobject target, jobject arg);
JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallObjectMethodV(JNIEnv* env, jobject thiz, jobject target, jobject arg);
JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallObjectMethodA(JNIEnv* env, jobject thiz, jobject target, jobject arg);

JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallBooleanMethod(JNIEnv* env, jobject thiz, jobject target, jboolean v);
JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallBooleanMethodV(JNIEnv* env, jobject thiz, jobject target, jboolean v);
JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallBooleanMethodA(JNIEnv* env, jobject thiz, jobject target, jboolean v);

JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallByteMethod(JNIEnv* env, jobject thiz, jobject target, jbyte v);
JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallByteMethodV(JNIEnv* env, jobject thiz, jobject target, jbyte v);
JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallByteMethodA(JNIEnv* env, jobject thiz, jobject target, jbyte v);

JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallCharMethod(JNIEnv* env, jobject thiz, jobject target, jchar v);
JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallCharMethodV(JNIEnv* env, jobject thiz, jobject target, jchar v);
JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallCharMethodA(JNIEnv* env, jobject thiz, jobject target, jchar v);

JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallShortMethod(JNIEnv* env, jobject thiz, jobject target, jshort v);
JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallShortMethodV(JNIEnv* env, jobject thiz, jobject target, jshort v);
JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallShortMethodA(JNIEnv* env, jobject thiz, jobject target, jshort v);

JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallIntMethod(JNIEnv* env, jobject thiz, jobject target, jint v);
JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallIntMethodV(JNIEnv* env, jobject thiz, jobject target, jint v);
JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallIntMethodA(JNIEnv* env, jobject thiz, jobject target, jint v);

JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallLongMethod(JNIEnv* env, jobject thiz, jobject target, jlong v);
JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallLongMethodV(JNIEnv* env, jobject thiz, jobject target, jlong v);
JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallLongMethodA(JNIEnv* env, jobject thiz, jobject target, jlong v);

JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallFloatMethod(JNIEnv* env, jobject thiz, jobject target, jfloat v);
JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallFloatMethodV(JNIEnv* env, jobject thiz, jobject target, jfloat v);
JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallFloatMethodA(JNIEnv* env, jobject thiz, jobject target, jfloat v);

JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallDoubleMethod(JNIEnv* env, jobject thiz, jobject target, jdouble v);
JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallDoubleMethodV(JNIEnv* env, jobject thiz, jobject target, jdouble v);
JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallDoubleMethodA(JNIEnv* env, jobject thiz, jobject target, jdouble v);

JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallVoidMethod(JNIEnv* env, jobject thiz, jobject target, jint marker);
JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallVoidMethodV(JNIEnv* env, jobject thiz, jobject target, jint marker);
JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallVoidMethodA(JNIEnv* env, jobject thiz, jobject target, jint marker);

JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallNonvirtualObjectMethod(JNIEnv* env, jobject thiz, jobject target, jobject arg);
JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallNonvirtualObjectMethodV(JNIEnv* env, jobject thiz, jobject target, jobject arg);
JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallNonvirtualObjectMethodA(JNIEnv* env, jobject thiz, jobject target, jobject arg);

JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallNonvirtualBooleanMethod(JNIEnv* env, jobject thiz, jobject target, jboolean v);
JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallNonvirtualBooleanMethodV(JNIEnv* env, jobject thiz, jobject target, jboolean v);
JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallNonvirtualBooleanMethodA(JNIEnv* env, jobject thiz, jobject target, jboolean v);

JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallNonvirtualByteMethod(JNIEnv* env, jobject thiz, jobject target, jbyte v);
JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallNonvirtualByteMethodV(JNIEnv* env, jobject thiz, jobject target, jbyte v);
JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallNonvirtualByteMethodA(JNIEnv* env, jobject thiz, jobject target, jbyte v);

JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallNonvirtualCharMethod(JNIEnv* env, jobject thiz, jobject target, jchar v);
JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallNonvirtualCharMethodV(JNIEnv* env, jobject thiz, jobject target, jchar v);
JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallNonvirtualCharMethodA(JNIEnv* env, jobject thiz, jobject target, jchar v);

JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallNonvirtualShortMethod(JNIEnv* env, jobject thiz, jobject target, jshort v);
JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallNonvirtualShortMethodV(JNIEnv* env, jobject thiz, jobject target, jshort v);
JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallNonvirtualShortMethodA(JNIEnv* env, jobject thiz, jobject target, jshort v);

JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallNonvirtualIntMethod(JNIEnv* env, jobject thiz, jobject target, jint v);
JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallNonvirtualIntMethodV(JNIEnv* env, jobject thiz, jobject target, jint v);
JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallNonvirtualIntMethodA(JNIEnv* env, jobject thiz, jobject target, jint v);

JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallNonvirtualLongMethod(JNIEnv* env, jobject thiz, jobject target, jlong v);
JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallNonvirtualLongMethodV(JNIEnv* env, jobject thiz, jobject target, jlong v);
JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallNonvirtualLongMethodA(JNIEnv* env, jobject thiz, jobject target, jlong v);

JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallNonvirtualFloatMethod(JNIEnv* env, jobject thiz, jobject target, jfloat v);
JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallNonvirtualFloatMethodV(JNIEnv* env, jobject thiz, jobject target, jfloat v);
JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallNonvirtualFloatMethodA(JNIEnv* env, jobject thiz, jobject target, jfloat v);

JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallNonvirtualDoubleMethod(JNIEnv* env, jobject thiz, jobject target, jdouble v);
JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallNonvirtualDoubleMethodV(JNIEnv* env, jobject thiz, jobject target, jdouble v);
JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallNonvirtualDoubleMethodA(JNIEnv* env, jobject thiz, jobject target, jdouble v);

JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallNonvirtualVoidMethod(JNIEnv* env, jobject thiz, jobject target, jint marker);
JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallNonvirtualVoidMethodV(JNIEnv* env, jobject thiz, jobject target, jint marker);
JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallNonvirtualVoidMethodA(JNIEnv* env, jobject thiz, jobject target, jint marker);

#endif