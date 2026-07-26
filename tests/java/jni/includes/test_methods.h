#ifndef TEST_METHODS_H
#define TEST_METHODS_H

#include "jni_common.h"

// Call<X>Method tests
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

// CallNonVirtual<X>Method tests
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

// CallStatic<X>Method tests
JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallStaticObjectMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jobject arg);
JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallStaticObjectMethodV(JNIEnv* env, jclass callerClass, jclass targetClass, jobject arg);
JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallStaticObjectMethodA(JNIEnv* env, jclass callerClass, jclass targetClass, jobject arg);

JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallStaticBooleanMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jboolean arg);
JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallStaticBooleanMethodV(JNIEnv* env, jclass callerClass, jclass targetClass, jboolean arg);
JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallStaticBooleanMethodA(JNIEnv* env, jclass callerClass, jclass targetClass, jboolean arg);

JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallStaticByteMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jbyte arg);
JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallStaticByteMethodV(JNIEnv* env, jclass callerClass, jclass targetClass, jbyte arg);
JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallStaticByteMethodA(JNIEnv* env, jclass callerClass, jclass targetClass, jbyte arg);

JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallStaticCharMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jchar arg);
JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallStaticCharMethodV(JNIEnv* env, jclass callerClass, jclass targetClass, jchar arg);
JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallStaticCharMethodA(JNIEnv* env, jclass callerClass, jclass targetClass, jchar arg);

JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallStaticShortMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jshort arg);
JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallStaticShortMethodV(JNIEnv* env, jclass callerClass, jclass targetClass, jshort arg);
JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallStaticShortMethodA(JNIEnv* env, jclass callerClass, jclass targetClass, jshort arg);

JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallStaticIntMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jint arg);
JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallStaticIntMethodV(JNIEnv* env, jclass callerClass, jclass targetClass, jint arg);
JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallStaticIntMethodA(JNIEnv* env, jclass callerClass, jclass targetClass, jint arg);

JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallStaticLongMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jlong arg);
JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallStaticLongMethodV(JNIEnv* env, jclass callerClass, jclass targetClass, jlong arg);
JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallStaticLongMethodA(JNIEnv* env, jclass callerClass, jclass targetClass, jlong arg);

JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallStaticFloatMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jfloat arg);
JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallStaticFloatMethodV(JNIEnv* env, jclass callerClass, jclass targetClass, jfloat arg);
JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallStaticFloatMethodA(JNIEnv* env, jclass callerClass, jclass targetClass, jfloat arg);

JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallStaticDoubleMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jdouble arg);
JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallStaticDoubleMethodV(JNIEnv* env, jclass callerClass, jclass targetClass, jdouble arg);
JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallStaticDoubleMethodA(JNIEnv* env, jclass callerClass, jclass targetClass, jdouble arg);

JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallStaticVoidMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jint arg);
JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallStaticVoidMethodV(JNIEnv* env, jclass callerClass, jclass targetClass, jint arg);
JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallStaticVoidMethodA(JNIEnv* env, jclass callerClass, jclass targetClass, jint arg);


#endif