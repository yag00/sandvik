#include "test_fields.h"

JNIEXPORT jint JNICALL Java_TestJNIFields_nativeGetIntField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "intValue", "I");
    return (*env)->GetIntField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetIntField(JNIEnv* env, jobject thiz, jobject obj, jint v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "intValue", "I");
    (*env)->SetIntField(env, obj, fid, v);
}

JNIEXPORT jboolean JNICALL Java_TestJNIFields_nativeGetBooleanField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "booleanValue", "Z");
    return (*env)->GetBooleanField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetBooleanField(JNIEnv* env, jobject thiz, jobject obj, jboolean v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "booleanValue", "Z");
    (*env)->SetBooleanField(env, obj, fid, v);
}

JNIEXPORT jbyte JNICALL Java_TestJNIFields_nativeGetByteField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "byteValue", "B");
    return (*env)->GetByteField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetByteField(JNIEnv* env, jobject thiz, jobject obj, jbyte v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "byteValue", "B");
    (*env)->SetByteField(env, obj, fid, v);
}

JNIEXPORT jchar JNICALL Java_TestJNIFields_nativeGetCharField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "charValue", "C");
    return (*env)->GetCharField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetCharField(JNIEnv* env, jobject thiz, jobject obj, jchar v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "charValue", "C");
    (*env)->SetCharField(env, obj, fid, v);
}

JNIEXPORT jshort JNICALL Java_TestJNIFields_nativeGetShortField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "shortValue", "S");
    return (*env)->GetShortField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetShortField(JNIEnv* env, jobject thiz, jobject obj, jshort v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "shortValue", "S");
    (*env)->SetShortField(env, obj, fid, v);
}

JNIEXPORT jlong JNICALL Java_TestJNIFields_nativeGetLongField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "longValue", "J");
    return (*env)->GetLongField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetLongField(JNIEnv* env, jobject thiz, jobject obj, jlong v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "longValue", "J");
    (*env)->SetLongField(env, obj, fid, v);
}

JNIEXPORT jfloat JNICALL Java_TestJNIFields_nativeGetFloatField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "floatValue", "F");
    return (*env)->GetFloatField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetFloatField(JNIEnv* env, jobject thiz, jobject obj, jfloat v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "floatValue", "F");
    (*env)->SetFloatField(env, obj, fid, v);
}

JNIEXPORT jdouble JNICALL Java_TestJNIFields_nativeGetDoubleField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "doubleValue", "D");
    return (*env)->GetDoubleField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetDoubleField(JNIEnv* env, jobject thiz, jobject obj, jdouble v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "doubleValue", "D");
    (*env)->SetDoubleField(env, obj, fid, v);
}

JNIEXPORT jobject JNICALL Java_TestJNIFields_nativeGetStringField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "stringValue", "Ljava/lang/String;");
    return (*env)->GetObjectField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStringField(JNIEnv* env, jobject thiz, jobject obj, jobject v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "stringValue", "Ljava/lang/String;");
    (*env)->SetObjectField(env, obj, fid, v);
}

JNIEXPORT jintArray JNICALL Java_TestJNIFields_nativeGetIntArrayField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "intArrayValue", "[I");
    return (jintArray)(*env)->GetObjectField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetIntArrayField(JNIEnv* env, jobject thiz, jobject obj, jintArray v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "intArrayValue", "[I");
    (*env)->SetObjectField(env, obj, fid, v);
}

JNIEXPORT jint JNICALL Java_TestJNIFields_nativeGetStaticIntField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "intStaticValue", "I");
    return (*env)->GetStaticIntField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticIntField(JNIEnv* env, jclass clazz, jint v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "intStaticValue", "I");
    (*env)->SetStaticIntField(env, clazz, fid, v);
}

JNIEXPORT jdouble JNICALL Java_TestJNIFields_nativeGetStaticDoubleField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "doubleStaticValue", "D");
    return (*env)->GetStaticDoubleField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticDoubleField(JNIEnv* env, jclass clazz, jdouble v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "doubleStaticValue", "D");
    (*env)->SetStaticDoubleField(env, clazz, fid, v);
}

JNIEXPORT jfloat JNICALL Java_TestJNIFields_nativeGetStaticFloatField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "floatStaticValue", "F");
    return (*env)->GetStaticFloatField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticFloatField(JNIEnv* env, jclass clazz, jfloat v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "floatStaticValue", "F");
    (*env)->SetStaticFloatField(env, clazz, fid, v);
}

JNIEXPORT jlong JNICALL Java_TestJNIFields_nativeGetStaticLongField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "longStaticValue", "J");
    return (*env)->GetStaticLongField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticLongField(JNIEnv* env, jclass clazz, jlong v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "longStaticValue", "J");
    (*env)->SetStaticLongField(env, clazz, fid, v);
}

JNIEXPORT jshort JNICALL Java_TestJNIFields_nativeGetStaticShortField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "shortStaticValue", "S");
    return (*env)->GetStaticShortField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticShortField(JNIEnv* env, jclass clazz, jshort v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "shortStaticValue", "S");
    (*env)->SetStaticShortField(env, clazz, fid, v);
}

JNIEXPORT jbyte JNICALL Java_TestJNIFields_nativeGetStaticByteField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "byteStaticValue", "B");
    return (*env)->GetStaticByteField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticByteField(JNIEnv* env, jclass clazz, jbyte v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "byteStaticValue", "B");
    (*env)->SetStaticByteField(env, clazz, fid, v);
}

JNIEXPORT jchar JNICALL Java_TestJNIFields_nativeGetStaticCharField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "charStaticValue", "C");
    return (*env)->GetStaticCharField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticCharField(JNIEnv* env, jclass clazz, jchar v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "charStaticValue", "C");
    (*env)->SetStaticCharField(env, clazz, fid, v);
}

JNIEXPORT jboolean JNICALL Java_TestJNIFields_nativeGetStaticBooleanField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "booleanStaticValue", "Z");
    return (*env)->GetStaticBooleanField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticBooleanField(JNIEnv* env, jclass clazz, jboolean v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "booleanStaticValue", "Z");
    (*env)->SetStaticBooleanField(env, clazz, fid, v);
}

JNIEXPORT jstring JNICALL Java_TestJNIFields_nativeGetStaticStringField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "stringStaticValue", "Ljava/lang/String;");
    return (jstring)(*env)->GetStaticObjectField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticStringField(JNIEnv* env, jclass clazz, jstring v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "stringStaticValue", "Ljava/lang/String;");
    (*env)->SetStaticObjectField(env, clazz, fid, v);
}

JNIEXPORT jintArray JNICALL Java_TestJNIFields_nativeGetStaticIntArrayField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "intArrayStaticValue", "[I");
    return (jintArray)(*env)->GetStaticObjectField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNIFields_nativeSetStaticIntArrayField(JNIEnv* env, jclass clazz, jintArray v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "intArrayStaticValue", "[I");
    (*env)->SetStaticObjectField(env, clazz, fid, v);
}