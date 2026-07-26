#include "test_methods.h"

// Call<X>Method tests
// Object
static jobject callObjectMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jobject res = (*env)->CallObjectMethodV(env, obj, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallObjectMethod(JNIEnv* env, jobject thiz, jobject target, jobject arg) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityObject", "(Ljava/lang/Object;)Ljava/lang/Object;");
    jobject res = (*env)->CallObjectMethod(env, target, mid, arg);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallObjectMethodV(JNIEnv* env, jobject thiz, jobject target, jobject arg) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityObject", "(Ljava/lang/Object;)Ljava/lang/Object;");
    jobject res = callObjectMethodV_helper(env, target, mid, arg);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallObjectMethodA(JNIEnv* env, jobject thiz, jobject target, jobject arg) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityObject", "(Ljava/lang/Object;)Ljava/lang/Object;");
    jvalue a[1]; a[0].l = arg;
    jobject res = (*env)->CallObjectMethodA(env, target, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Boolean
static jboolean callBooleanMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jboolean res = (*env)->CallBooleanMethodV(env, obj, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallBooleanMethod(JNIEnv* env, jobject thiz, jobject target, jboolean v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityBoolean", "(Z)Z");
    jboolean res = (*env)->CallBooleanMethod(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallBooleanMethodV(JNIEnv* env, jobject thiz, jobject target, jboolean v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityBoolean", "(Z)Z");
    jboolean res = callBooleanMethodV_helper(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallBooleanMethodA(JNIEnv* env, jobject thiz, jobject target, jboolean v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityBoolean", "(Z)Z");
    jvalue a[1]; a[0].z = v;
    jboolean res = (*env)->CallBooleanMethodA(env, target, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Byte
static jbyte callByteMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jbyte res = (*env)->CallByteMethodV(env, obj, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallByteMethod(JNIEnv* env, jobject thiz, jobject target, jbyte v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityByte", "(B)B");
    jbyte res = (*env)->CallByteMethod(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallByteMethodV(JNIEnv* env, jobject thiz, jobject target, jbyte v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityByte", "(B)B");
    jbyte res = callByteMethodV_helper(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallByteMethodA(JNIEnv* env, jobject thiz, jobject target, jbyte v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityByte", "(B)B");
    jvalue a[1]; a[0].b = v;
    jbyte res = (*env)->CallByteMethodA(env, target, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Char
static jchar callCharMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jchar res = (*env)->CallCharMethodV(env, obj, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallCharMethod(JNIEnv* env, jobject thiz, jobject target, jchar v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityChar", "(C)C");
    jchar res = (*env)->CallCharMethod(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallCharMethodV(JNIEnv* env, jobject thiz, jobject target, jchar v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityChar", "(C)C");
    jchar res = callCharMethodV_helper(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallCharMethodA(JNIEnv* env, jobject thiz, jobject target, jchar v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityChar", "(C)C");
    jvalue a[1]; a[0].c = v;
    jchar res = (*env)->CallCharMethodA(env, target, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Short
static jshort callShortMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jshort res = (*env)->CallShortMethodV(env, obj, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallShortMethod(JNIEnv* env, jobject thiz, jobject target, jshort v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityShort", "(S)S");
    jshort res = (*env)->CallShortMethod(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallShortMethodV(JNIEnv* env, jobject thiz, jobject target, jshort v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityShort", "(S)S");
    jshort res = callShortMethodV_helper(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallShortMethodA(JNIEnv* env, jobject thiz, jobject target, jshort v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityShort", "(S)S");
    jvalue a[1]; a[0].s = v;
    jshort res = (*env)->CallShortMethodA(env, target, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Int
static jint callIntMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jint res = (*env)->CallIntMethodV(env, obj, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallIntMethod(JNIEnv* env, jobject thiz, jobject target, jint v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityInt", "(I)I");
    jint res = (*env)->CallIntMethod(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallIntMethodV(JNIEnv* env, jobject thiz, jobject target, jint v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityInt", "(I)I");
    jint res = callIntMethodV_helper(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallIntMethodA(JNIEnv* env, jobject thiz, jobject target, jint v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityInt", "(I)I");
    jvalue a[1]; a[0].i = v;
    jint res = (*env)->CallIntMethodA(env, target, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Long
static jlong callLongMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jlong res = (*env)->CallLongMethodV(env, obj, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallLongMethod(JNIEnv* env, jobject thiz, jobject target, jlong v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityLong", "(J)J");
    jlong res = (*env)->CallLongMethod(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallLongMethodV(JNIEnv* env, jobject thiz, jobject target, jlong v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityLong", "(J)J");
    jlong res = callLongMethodV_helper(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallLongMethodA(JNIEnv* env, jobject thiz, jobject target, jlong v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityLong", "(J)J");
    jvalue a[1]; a[0].j = v;
    jlong res = (*env)->CallLongMethodA(env, target, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Float
static jfloat callFloatMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jfloat res = (*env)->CallFloatMethodV(env, obj, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallFloatMethod(JNIEnv* env, jobject thiz, jobject target, jfloat v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityFloat", "(F)F");
    jfloat res = (*env)->CallFloatMethod(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallFloatMethodV(JNIEnv* env, jobject thiz, jobject target, jfloat v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityFloat", "(F)F");
    jfloat res = callFloatMethodV_helper(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallFloatMethodA(JNIEnv* env, jobject thiz, jobject target, jfloat v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityFloat", "(F)F");
    jvalue a[1]; a[0].f = v;
    jfloat res = (*env)->CallFloatMethodA(env, target, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Double
static jdouble callDoubleMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jdouble res = (*env)->CallDoubleMethodV(env, obj, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallDoubleMethod(JNIEnv* env, jobject thiz, jobject target, jdouble v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityDouble", "(D)D");
    jdouble res = (*env)->CallDoubleMethod(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallDoubleMethodV(JNIEnv* env, jobject thiz, jobject target, jdouble v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityDouble", "(D)D");
    jdouble res = callDoubleMethodV_helper(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallDoubleMethodA(JNIEnv* env, jobject thiz, jobject target, jdouble v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityDouble", "(D)D");
    jvalue a[1]; a[0].d = v;
    jdouble res = (*env)->CallDoubleMethodA(env, target, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Void
static void callVoidMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    (*env)->CallVoidMethodV(env, obj, mid, ap);
    va_end(ap);
}
JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallVoidMethod(JNIEnv* env, jobject thiz, jobject target, jint marker) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityVoid", "(I)V");
    (*env)->CallVoidMethod(env, target, mid, marker);
    (*env)->DeleteLocalRef(env, cls);
}
JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallVoidMethodV(JNIEnv* env, jobject thiz, jobject target, jint marker) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityVoid", "(I)V");
    callVoidMethodV_helper(env, target, mid, marker);
    (*env)->DeleteLocalRef(env, cls);
}
JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallVoidMethodA(JNIEnv* env, jobject thiz, jobject target, jint marker) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityVoid", "(I)V");
    jvalue a[1]; a[0].i = marker;
    (*env)->CallVoidMethodA(env, target, mid, a);
    (*env)->DeleteLocalRef(env, cls);
}

// CallNonvirtual<X>Method tests
// Object
static jobject callNonvirtualObjectMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jobject res = (*env)->CallNonvirtualObjectMethodV(env, obj, (*env)->GetObjectClass(env, obj), mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallNonvirtualObjectMethod(JNIEnv* env, jobject thiz, jobject target, jobject arg) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityObject", "(Ljava/lang/Object;)Ljava/lang/Object;");
    jobject res = (*env)->CallNonvirtualObjectMethod(env, target, cls, mid, arg);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallNonvirtualObjectMethodV(JNIEnv* env, jobject thiz, jobject target, jobject arg) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityObject", "(Ljava/lang/Object;)Ljava/lang/Object;");
    jobject res = callNonvirtualObjectMethodV_helper(env, target, mid, arg);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallNonvirtualObjectMethodA(JNIEnv* env, jobject thiz, jobject target, jobject arg) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityObject", "(Ljava/lang/Object;)Ljava/lang/Object;");
    jvalue a[1]; a[0].l = arg;
    jobject res = (*env)->CallNonvirtualObjectMethodA(env, target, cls, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Boolean
static jboolean callNonvirtualBooleanMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jboolean res = (*env)->CallNonvirtualBooleanMethodV(env, obj, (*env)->GetObjectClass(env, obj), mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallNonvirtualBooleanMethod(JNIEnv* env, jobject thiz, jobject target, jboolean v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityBoolean", "(Z)Z");
    jboolean res = (*env)->CallNonvirtualBooleanMethod(env, target, cls, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallNonvirtualBooleanMethodV(JNIEnv* env, jobject thiz, jobject target, jboolean v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityBoolean", "(Z)Z");
    jboolean res = callNonvirtualBooleanMethodV_helper(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallNonvirtualBooleanMethodA(JNIEnv* env, jobject thiz, jobject target, jboolean v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityBoolean", "(Z)Z");
    jvalue a[1]; a[0].z = v;
    jboolean res = (*env)->CallNonvirtualBooleanMethodA(env, target, cls, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Byte
static jbyte callNonvirtualByteMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jbyte res = (*env)->CallNonvirtualByteMethodV(env, obj, (*env)->GetObjectClass(env, obj), mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallNonvirtualByteMethod(JNIEnv* env, jobject thiz, jobject target, jbyte v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityByte", "(B)B");
    jbyte res = (*env)->CallNonvirtualByteMethod(env, target, cls, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallNonvirtualByteMethodV(JNIEnv* env, jobject thiz, jobject target, jbyte v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityByte", "(B)B");
    jbyte res = callNonvirtualByteMethodV_helper(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallNonvirtualByteMethodA(JNIEnv* env, jobject thiz, jobject target, jbyte v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityByte", "(B)B");
    jvalue a[1]; a[0].b = v;
    jbyte res = (*env)->CallNonvirtualByteMethodA(env, target, cls, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Char
static jchar callNonvirtualCharMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jchar res = (*env)->CallNonvirtualCharMethodV(env, obj, (*env)->GetObjectClass(env, obj), mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallNonvirtualCharMethod(JNIEnv* env, jobject thiz, jobject target, jchar v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityChar", "(C)C");
    jchar res = (*env)->CallNonvirtualCharMethod(env, target, cls, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallNonvirtualCharMethodV(JNIEnv* env, jobject thiz, jobject target, jchar v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityChar", "(C)C");
    jchar res = callNonvirtualCharMethodV_helper(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallNonvirtualCharMethodA(JNIEnv* env, jobject thiz, jobject target, jchar v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityChar", "(C)C");
    jvalue a[1]; a[0].c = v;
    jchar res = (*env)->CallNonvirtualCharMethodA(env, target, cls, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Short
static jshort callNonvirtualShortMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jshort res = (*env)->CallNonvirtualShortMethodV(env, obj, (*env)->GetObjectClass(env, obj), mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallNonvirtualShortMethod(JNIEnv* env, jobject thiz, jobject target, jshort v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityShort", "(S)S");
    jshort res = (*env)->CallNonvirtualShortMethod(env, target, cls, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallNonvirtualShortMethodV(JNIEnv* env, jobject thiz, jobject target, jshort v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityShort", "(S)S");
    jshort res = callNonvirtualShortMethodV_helper(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallNonvirtualShortMethodA(JNIEnv* env, jobject thiz, jobject target, jshort v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityShort", "(S)S");
    jvalue a[1]; a[0].s = v;
    jshort res = (*env)->CallNonvirtualShortMethodA(env, target, cls, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Int
static jint callNonvirtualIntMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jint res = (*env)->CallNonvirtualIntMethodV(env, obj, (*env)->GetObjectClass(env, obj), mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallNonvirtualIntMethod(JNIEnv* env, jobject thiz, jobject target, jint v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityInt", "(I)I");
    jint res = (*env)->CallNonvirtualIntMethod(env, target, cls, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallNonvirtualIntMethodV(JNIEnv* env, jobject thiz, jobject target, jint v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityInt", "(I)I");
    jint res = callNonvirtualIntMethodV_helper(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallNonvirtualIntMethodA(JNIEnv* env, jobject thiz, jobject target, jint v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityInt", "(I)I");
    jvalue a[1]; a[0].i = v;
    jint res = (*env)->CallNonvirtualIntMethodA(env, target, cls, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Long
static jlong callNonvirtualLongMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jlong res = (*env)->CallNonvirtualLongMethodV(env, obj, (*env)->GetObjectClass(env, obj), mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallNonvirtualLongMethod(JNIEnv* env, jobject thiz, jobject target, jlong v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityLong", "(J)J");
    jlong res = (*env)->CallNonvirtualLongMethod(env, target, cls, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallNonvirtualLongMethodV(JNIEnv* env, jobject thiz, jobject target, jlong v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityLong", "(J)J");
    jlong res = callNonvirtualLongMethodV_helper(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallNonvirtualLongMethodA(JNIEnv* env, jobject thiz, jobject target, jlong v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityLong", "(J)J");
    jvalue a[1]; a[0].j = v;
    jlong res = (*env)->CallNonvirtualLongMethodA(env, target, cls, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Float
static jfloat callNonvirtualFloatMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jfloat res = (*env)->CallNonvirtualFloatMethodV(env, obj, (*env)->GetObjectClass(env, obj), mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallNonvirtualFloatMethod(JNIEnv* env, jobject thiz, jobject target, jfloat v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityFloat", "(F)F");
    jfloat res = (*env)->CallNonvirtualFloatMethod(env, target, cls, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallNonvirtualFloatMethodV(JNIEnv* env, jobject thiz, jobject target, jfloat v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityFloat", "(F)F");
    jfloat res = callNonvirtualFloatMethodV_helper(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallNonvirtualFloatMethodA(JNIEnv* env, jobject thiz, jobject target, jfloat v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityFloat", "(F)F");
    jvalue a[1]; a[0].f = v;
    jfloat res = (*env)->CallNonvirtualFloatMethodA(env, target, cls, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Double
static jdouble callNonvirtualDoubleMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jdouble res = (*env)->CallNonvirtualDoubleMethodV(env, obj, (*env)->GetObjectClass(env, obj), mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallNonvirtualDoubleMethod(JNIEnv* env, jobject thiz, jobject target, jdouble v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityDouble", "(D)D");
    jdouble res = (*env)->CallNonvirtualDoubleMethod(env, target, cls, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallNonvirtualDoubleMethodV(JNIEnv* env, jobject thiz, jobject target, jdouble v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityDouble", "(D)D");
    jdouble res = callNonvirtualDoubleMethodV_helper(env, target, mid, v);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}
JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallNonvirtualDoubleMethodA(JNIEnv* env, jobject thiz, jobject target, jdouble v) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityDouble", "(D)D");
    jvalue a[1]; a[0].d = v;
    jdouble res = (*env)->CallNonvirtualDoubleMethodA(env, target, cls, mid, a);
    (*env)->DeleteLocalRef(env, cls);
    return res;
}

// Void
static void callNonvirtualVoidMethodV_helper(JNIEnv* env, jobject obj, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    (*env)->CallNonvirtualVoidMethodV(env, obj, (*env)->GetObjectClass(env, obj), mid, ap);
    va_end(ap);
}
JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallNonvirtualVoidMethod(JNIEnv* env, jobject thiz, jobject target, jint marker) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityVoid", "(I)V");
    (*env)->CallNonvirtualVoidMethod(env, target, cls, mid, marker);
    (*env)->DeleteLocalRef(env, cls);
}
JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallNonvirtualVoidMethodV(JNIEnv* env, jobject thiz, jobject target, jint marker) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityVoid", "(I)V");
    callNonvirtualVoidMethodV_helper(env, target, mid, marker);
    (*env)->DeleteLocalRef(env, cls);
}
JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallNonvirtualVoidMethodA(JNIEnv* env, jobject thiz, jobject target, jint marker) {
    jclass cls = (*env)->GetObjectClass(env, target);
    jmethodID mid = (*env)->GetMethodID(env, cls, "identityVoid", "(I)V");
    jvalue a[1]; a[0].i = marker;
    (*env)->CallNonvirtualVoidMethodA(env, target, cls, mid, a);
    (*env)->DeleteLocalRef(env, cls);
}

// CallStatic<X>Method tests
// Object
static jobject callStaticObjectMethodV_helper(JNIEnv* env, jclass cls, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jobject res = (*env)->CallStaticObjectMethodV(env, cls, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallStaticObjectMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jobject arg) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityObject", "(Ljava/lang/Object;)Ljava/lang/Object;");
    jobject res = (*env)->CallStaticObjectMethod(env, targetClass, mid, arg);
    return res;
}
JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallStaticObjectMethodV(JNIEnv* env, jclass callerClass, jclass targetClass, jobject arg) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityObject", "(Ljava/lang/Object;)Ljava/lang/Object;");
    jobject res = callStaticObjectMethodV_helper(env, targetClass, mid, arg);
    return res;
}
JNIEXPORT jobject JNICALL Java_TestJNIMethods_nativeCallStaticObjectMethodA(JNIEnv* env, jclass callerClass, jclass targetClass, jobject arg) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityObject", "(Ljava/lang/Object;)Ljava/lang/Object;");
    jvalue a[1]; a[0].l = arg;
    jobject res = (*env)->CallStaticObjectMethodA(env, targetClass, mid, a);
    return res;
}

// Boolean
static jboolean callStaticBooleanMethodV_helper(JNIEnv* env, jclass cls, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jboolean res = (*env)->CallStaticBooleanMethodV(env, cls, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallStaticBooleanMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jboolean v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityBoolean", "(Z)Z");
    jboolean res = (*env)->CallStaticBooleanMethod(env, targetClass, mid, v);
    return res;
}
JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallStaticBooleanMethodV(JNIEnv*env, jclass callerClass, jclass targetClass, jboolean v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityBoolean", "(Z)Z");
    jboolean res = callStaticBooleanMethodV_helper(env, targetClass, mid, v);
    return res;
}
JNIEXPORT jboolean JNICALL Java_TestJNIMethods_nativeCallStaticBooleanMethodA(JNIEnv*env, jclass callerClass, jclass targetClass, jboolean v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityBoolean", "(Z)Z");
    jvalue a[1]; a[0].z = v;
    jboolean res = (*env)->CallStaticBooleanMethodA(env, targetClass, mid, a);
    return res;
}

// Byte
static jbyte callStaticByteMethodV_helper(JNIEnv* env, jclass cls, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jbyte res = (*env)->CallStaticByteMethodV(env, cls, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallStaticByteMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jbyte v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityByte", "(B)B");
    jbyte res = (*env)->CallStaticByteMethod(env, targetClass, mid, v);
    return res;
}
JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallStaticByteMethodV(JNIEnv*env, jclass callerClass, jclass targetClass, jbyte v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityByte", "(B)B");
    jbyte res = callStaticByteMethodV_helper(env, targetClass, mid, v);
    return res;
}
JNIEXPORT jbyte JNICALL Java_TestJNIMethods_nativeCallStaticByteMethodA(JNIEnv*env, jclass callerClass, jclass targetClass, jbyte v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityByte", "(B)B");
    jvalue a[1]; a[0].b = v;
    jbyte res = (*env)->CallStaticByteMethodA(env, targetClass, mid, a);
    return res;
}

// Char
static jchar callStaticCharMethodV_helper(JNIEnv* env, jclass cls, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jchar res = (*env)->CallStaticCharMethodV(env, cls, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallStaticCharMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jchar v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityChar", "(C)C");
    jchar res = (*env)->CallStaticCharMethod(env, targetClass, mid, v);
    return res;
}
JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallStaticCharMethodV(JNIEnv*env, jclass callerClass, jclass targetClass, jchar v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityChar", "(C)C");
    jchar res = callStaticCharMethodV_helper(env, targetClass, mid, v);
    return res;
}
JNIEXPORT jchar JNICALL Java_TestJNIMethods_nativeCallStaticCharMethodA(JNIEnv*env, jclass callerClass, jclass targetClass, jchar v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityChar", "(C)C");
    jvalue a[1]; a[0].c = v;
    jchar res = (*env)->CallStaticCharMethodA(env, targetClass, mid, a);
    return res;
}

// Short
static jshort callStaticShortMethodV_helper(JNIEnv* env, jclass cls, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jshort res = (*env)->CallStaticShortMethodV(env, cls, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallStaticShortMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jshort v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityShort", "(S)S");
    jshort res = (*env)->CallStaticShortMethod(env, targetClass, mid, v);
    return res;
}
JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallStaticShortMethodV(JNIEnv*env, jclass callerClass, jclass targetClass, jshort v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityShort", "(S)S");
    jshort res = callStaticShortMethodV_helper(env, targetClass, mid, v);
    return res;
}
JNIEXPORT jshort JNICALL Java_TestJNIMethods_nativeCallStaticShortMethodA(JNIEnv*env, jclass callerClass, jclass targetClass, jshort v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityShort", "(S)S");
    jvalue a[1]; a[0].s = v;
    jshort res = (*env)->CallStaticShortMethodA(env, targetClass, mid, a);
    return res;
}

// Int
static jint callStaticIntMethodV_helper(JNIEnv* env, jclass cls, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jint res = (*env)->CallStaticIntMethodV(env, cls, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallStaticIntMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jint v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityInt", "(I)I");
    jint res = (*env)->CallStaticIntMethod(env, targetClass, mid, v);
    return res;
}
JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallStaticIntMethodV(JNIEnv*env, jclass callerClass, jclass targetClass, jint v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityInt", "(I)I");
    jint res = callStaticIntMethodV_helper(env, targetClass, mid, v);
    return res;
}
JNIEXPORT jint JNICALL Java_TestJNIMethods_nativeCallStaticIntMethodA(JNIEnv*env, jclass callerClass, jclass targetClass, jint v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityInt", "(I)I");
    jvalue a[1]; a[0].i = v;
    jint res = (*env)->CallStaticIntMethodA(env, targetClass, mid, a);
    return res;
}

// Long
static jlong callStaticLongMethodV_helper(JNIEnv* env, jclass cls, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jlong res = (*env)->CallStaticLongMethodV(env, cls, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallStaticLongMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jlong v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityLong", "(J)J");
    jlong res = (*env)->CallStaticLongMethod(env, targetClass, mid, v);
    return res;
}
JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallStaticLongMethodV(JNIEnv*env, jclass callerClass, jclass targetClass, jlong v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityLong", "(J)J");
    jlong res = callStaticLongMethodV_helper(env, targetClass, mid, v);
    return res;
}
JNIEXPORT jlong JNICALL Java_TestJNIMethods_nativeCallStaticLongMethodA(JNIEnv*env, jclass callerClass, jclass targetClass, jlong v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityLong", "(J)J");
    jvalue a[1]; a[0].j = v;
    jlong res = (*env)->CallStaticLongMethodA(env, targetClass, mid, a);
    return res;
}

// Float
static jfloat callStaticFloatMethodV_helper(JNIEnv* env, jclass cls, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jfloat res = (*env)->CallStaticFloatMethodV(env, cls, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallStaticFloatMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jfloat v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityFloat", "(F)F");
    jfloat res = (*env)->CallStaticFloatMethod(env, targetClass, mid, v);
    return res;
}
JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallStaticFloatMethodV(JNIEnv*env, jclass callerClass, jclass targetClass, jfloat v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityFloat", "(F)F");
    jfloat res = callStaticFloatMethodV_helper(env, targetClass, mid, v);
    return res;
}
JNIEXPORT jfloat JNICALL Java_TestJNIMethods_nativeCallStaticFloatMethodA(JNIEnv*env, jclass callerClass, jclass targetClass, jfloat v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityFloat", "(F)F");
    jvalue a[1]; a[0].f = v;
    jfloat res = (*env)->CallStaticFloatMethodA(env, targetClass, mid, a);
    return res;
}

// Double
static jdouble callStaticDoubleMethodV_helper(JNIEnv* env, jclass cls, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    jdouble res = (*env)->CallStaticDoubleMethodV(env, cls, mid, ap);
    va_end(ap);
    return res;
}
JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallStaticDoubleMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jdouble v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityDouble", "(D)D");
    jdouble res = (*env)->CallStaticDoubleMethod(env, targetClass, mid, v);
    return res;
}
JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallStaticDoubleMethodV(JNIEnv*env, jclass callerClass, jclass targetClass, jdouble v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityDouble", "(D)D");
    jdouble res = callStaticDoubleMethodV_helper(env, targetClass, mid, v);
    return res;
}
JNIEXPORT jdouble JNICALL Java_TestJNIMethods_nativeCallStaticDoubleMethodA(JNIEnv*env, jclass callerClass, jclass targetClass, jdouble v) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityDouble", "(D)D");
    jvalue a[1]; a[0].d = v;
    jdouble res = (*env)->CallStaticDoubleMethodA(env, targetClass, mid, a);
    return res;
}

// Void
static void callStaticVoidMethodV_helper(JNIEnv* env, jclass cls, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    (*env)->CallStaticVoidMethodV(env, cls, mid, ap);
    va_end(ap);
}
JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallStaticVoidMethod(JNIEnv* env, jclass callerClass, jclass targetClass, jint marker) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityVoid", "(I)V");
    (*env)->CallStaticVoidMethod(env, targetClass, mid, marker);
}
JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallStaticVoidMethodV(JNIEnv* env, jclass callerClass, jclass targetClass, jint marker) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityVoid", "(I)V");
    callStaticVoidMethodV_helper(env, targetClass, mid, marker);
}
JNIEXPORT void JNICALL Java_TestJNIMethods_nativeCallStaticVoidMethodA(JNIEnv* env, jclass callerClass, jclass targetClass, jint marker) {
    jmethodID mid = (*env)->GetStaticMethodID(env, targetClass, "staticIdentityVoid", "(I)V");
    jvalue a[1]; a[0].i = marker;
    (*env)->CallStaticVoidMethodA(env, targetClass, mid, a);
}