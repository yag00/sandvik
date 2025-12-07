// gcc -fPIC -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/linux" -shared -o libtestjni.so testjni.c

#include <jni.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

static jobject globalRef = NULL;
static jweak weakRef = NULL;

JNIEXPORT jstring JNICALL Java_TestJNI_nativeHello(JNIEnv* env, jobject thiz, jstring name) {
    const char* n = (*env)->GetStringUTFChars(env, name, NULL);
    char buff[256];
    int size = (int)(*env)->GetStringUTFLength(env, name);
    snprintf(buff, sizeof(buff), "Hello %s %d", n, size);
    (*env)->ReleaseStringUTFChars(env, name, n);
    return (*env)->NewStringUTF(env, buff);
}

JNIEXPORT jstring JNICALL Java_TestJNI_nativeUnicodeStringTest(JNIEnv* env, jobject thiz, jstring input) {
    // Get the length (in jchar, not bytes)
    jsize len = (*env)->GetStringLength(env, input);

    // Get the UTF-16 chars
    jboolean isCopy;
    const jchar* chars = (*env)->GetStringChars(env, input, &isCopy);

    // Create a new string from the same chars
    jstring out = (*env)->NewString(env, chars, len);

    // Release the chars
    (*env)->ReleaseStringChars(env, input, chars);

    return out;
}

JNIEXPORT jintArray JNICALL Java_TestJNI_nativeArraySum(JNIEnv* env, jobject thiz, jintArray arr) {
    jsize len = (*env)->GetArrayLength(env, arr);
    jint* data = (*env)->GetIntArrayElements(env, arr, NULL);
    jint sum = 0;
    for (int i = 0; i < len; i++) sum += data[i];
    (*env)->ReleaseIntArrayElements(env, arr, data, 0);

    jintArray out = (*env)->NewIntArray(env, 1);
    (*env)->SetIntArrayRegion(env, out, 0, 1, &sum);
    return out;
}

JNIEXPORT jint JNICALL Java_TestJNI_nativeGetIntField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "intValue", "I");
    return (*env)->GetIntField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetIntField(JNIEnv* env, jobject thiz, jobject obj, jint v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "intValue", "I");
    (*env)->SetIntField(env, obj, fid, v);
}

JNIEXPORT jboolean JNICALL Java_TestJNI_nativeGetBooleanField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "booleanValue", "Z");
    return (*env)->GetBooleanField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetBooleanField(JNIEnv* env, jobject thiz, jobject obj, jboolean v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "booleanValue", "Z");
    (*env)->SetBooleanField(env, obj, fid, v);
}

JNIEXPORT jbyte JNICALL Java_TestJNI_nativeGetByteField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "byteValue", "B");
    return (*env)->GetByteField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetByteField(JNIEnv* env, jobject thiz, jobject obj, jbyte v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "byteValue", "B");
    (*env)->SetByteField(env, obj, fid, v);
}

JNIEXPORT jchar JNICALL Java_TestJNI_nativeGetCharField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "charValue", "C");
    return (*env)->GetCharField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetCharField(JNIEnv* env, jobject thiz, jobject obj, jchar v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "charValue", "C");
    (*env)->SetCharField(env, obj, fid, v);
}

JNIEXPORT jshort JNICALL Java_TestJNI_nativeGetShortField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "shortValue", "S");
    return (*env)->GetShortField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetShortField(JNIEnv* env, jobject thiz, jobject obj, jshort v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "shortValue", "S");
    (*env)->SetShortField(env, obj, fid, v);
}

JNIEXPORT jlong JNICALL Java_TestJNI_nativeGetLongField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "longValue", "J");
    return (*env)->GetLongField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetLongField(JNIEnv* env, jobject thiz, jobject obj, jlong v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "longValue", "J");
    (*env)->SetLongField(env, obj, fid, v);
}

JNIEXPORT jfloat JNICALL Java_TestJNI_nativeGetFloatField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "floatValue", "F");
    return (*env)->GetFloatField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetFloatField(JNIEnv* env, jobject thiz, jobject obj, jfloat v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "floatValue", "F");
    (*env)->SetFloatField(env, obj, fid, v);
}

JNIEXPORT jdouble JNICALL Java_TestJNI_nativeGetDoubleField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "doubleValue", "D");
    return (*env)->GetDoubleField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetDoubleField(JNIEnv* env, jobject thiz, jobject obj, jdouble v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "doubleValue", "D");
    (*env)->SetDoubleField(env, obj, fid, v);
}

// Object (String example)
JNIEXPORT jobject JNICALL Java_TestJNI_nativeGetStringField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "stringValue", "Ljava/lang/String;");
    return (*env)->GetObjectField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetStringField(JNIEnv* env, jobject thiz, jobject obj, jobject v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "stringValue", "Ljava/lang/String;");
    (*env)->SetObjectField(env, obj, fid, v);
}

JNIEXPORT jintArray JNICALL Java_TestJNI_nativeGetIntArrayField(JNIEnv* env, jobject thiz, jobject obj) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "intArrayValue", "[I");
    return (jintArray)(*env)->GetObjectField(env, obj, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetIntArrayField(JNIEnv* env, jobject thiz, jobject obj, jintArray v) {
    jclass cls = (*env)->GetObjectClass(env, obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "intArrayValue", "[I");
    (*env)->SetObjectField(env, obj, fid, v);
}

JNIEXPORT jint JNICALL Java_TestJNI_nativeGetStaticIntField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "intStaticValue", "I");
    return (*env)->GetStaticIntField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetStaticIntField(JNIEnv* env, jclass clazz, jint v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "intStaticValue", "I");
    (*env)->SetStaticIntField(env, clazz, fid, v);
}

JNIEXPORT jdouble JNICALL Java_TestJNI_nativeGetStaticDoubleField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "doubleStaticValue", "D");
    return (*env)->GetStaticDoubleField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetStaticDoubleField(JNIEnv* env, jclass clazz, jdouble v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "doubleStaticValue", "D");
    (*env)->SetStaticDoubleField(env, clazz, fid, v);
}

JNIEXPORT jfloat JNICALL Java_TestJNI_nativeGetStaticFloatField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "floatStaticValue", "F");
    return (*env)->GetStaticFloatField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetStaticFloatField(JNIEnv* env, jclass clazz, jfloat v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "floatStaticValue", "F");
    (*env)->SetStaticFloatField(env, clazz, fid, v);
}

JNIEXPORT jlong JNICALL Java_TestJNI_nativeGetStaticLongField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "longStaticValue", "J");
    return (*env)->GetStaticLongField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetStaticLongField(JNIEnv* env, jclass clazz, jlong v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "longStaticValue", "J");
    (*env)->SetStaticLongField(env, clazz, fid, v);
}

JNIEXPORT jshort JNICALL Java_TestJNI_nativeGetStaticShortField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "shortStaticValue", "S");
    return (*env)->GetStaticShortField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetStaticShortField(JNIEnv* env, jclass clazz, jshort v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "shortStaticValue", "S");
    (*env)->SetStaticShortField(env, clazz, fid, v);
}

JNIEXPORT jbyte JNICALL Java_TestJNI_nativeGetStaticByteField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "byteStaticValue", "B");
    return (*env)->GetStaticByteField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetStaticByteField(JNIEnv* env, jclass clazz, jbyte v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "byteStaticValue", "B");
    (*env)->SetStaticByteField(env, clazz, fid, v);
}

JNIEXPORT jchar JNICALL Java_TestJNI_nativeGetStaticCharField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "charStaticValue", "C");
    return (*env)->GetStaticCharField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetStaticCharField(JNIEnv* env, jclass clazz, jchar v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "charStaticValue", "C");
    (*env)->SetStaticCharField(env, clazz, fid, v);
}

JNIEXPORT jboolean JNICALL Java_TestJNI_nativeGetStaticBooleanField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "booleanStaticValue", "Z");
    return (*env)->GetStaticBooleanField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetStaticBooleanField(JNIEnv* env, jclass clazz, jboolean v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "booleanStaticValue", "Z");
    (*env)->SetStaticBooleanField(env, clazz, fid, v);
}

JNIEXPORT jstring JNICALL Java_TestJNI_nativeGetStaticStringField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "stringStaticValue", "Ljava/lang/String;");
    return (jstring)(*env)->GetStaticObjectField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetStaticStringField(JNIEnv* env, jclass clazz, jstring v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "stringStaticValue", "Ljava/lang/String;");
    (*env)->SetStaticObjectField(env, clazz, fid, v);
}

JNIEXPORT jintArray JNICALL Java_TestJNI_nativeGetStaticIntArrayField(JNIEnv* env, jclass clazz) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "intArrayStaticValue", "[I");
    return (jintArray)(*env)->GetStaticObjectField(env, clazz, fid);
}

JNIEXPORT void JNICALL Java_TestJNI_nativeSetStaticIntArrayField(JNIEnv* env, jclass clazz, jintArray v) {
    jfieldID fid = (*env)->GetStaticFieldID(env, clazz, "intArrayStaticValue", "[I");
    (*env)->SetStaticObjectField(env, clazz, fid, v);
}

JNIEXPORT jstring JNICALL Java_TestJNI_nativeLocalRefTest(JNIEnv* env, jobject thiz) {
    for (int i = 0; i < 1000; i++) {
        jstring tmp = (*env)->NewStringUTF(env, "temp");
        // test local ref deletion
        (*env)->DeleteLocalRef(env, tmp);
    }
    return (*env)->NewStringUTF(env, "OK");
}

JNIEXPORT jstring JNICALL Java_TestJNI_nativeGlobalRefTest(JNIEnv* env, jobject thiz) {
    if (!globalRef) {
        globalRef = (*env)->NewGlobalRef(env, thiz);
    }

    if (globalRef == NULL) {
        return (*env)->NewStringUTF(env, "FAIL");
    }

    return (*env)->NewStringUTF(env, "OK");
}

JNIEXPORT jstring JNICALL Java_TestJNI_nativeWeakRefTest(JNIEnv* env, jobject thiz) {
    if (!weakRef) {
        weakRef = (*env)->NewWeakGlobalRef(env, thiz);
    }
    jobject strong = (*env)->NewLocalRef(env, weakRef);
    if (strong == NULL) {
        return (*env)->NewStringUTF(env, "FAIL");
    }
    (*env)->DeleteLocalRef(env, strong);
    return (*env)->NewStringUTF(env, "OK");
}

JNIEXPORT jboolean JNICALL Java_TestJNI_nativeThrowAndCatch(JNIEnv* env, jobject thiz) {
    jclass exc = (*env)->FindClass(env, "java/lang/RuntimeException");
    (*env)->ThrowNew(env, exc, "Test exception");

    if (!(*env)->ExceptionCheck(env)) return JNI_FALSE;

    (*env)->ExceptionClear(env);
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_TestJNI_nativeIsInstanceOf(JNIEnv* env, jobject thiz, jobject obj, jstring className) {
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

JNIEXPORT jobject JNICALL Java_TestJNI_nativeNewObject(JNIEnv* env, jobject thiz, jstring message, jint i) {
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

JNIEXPORT jobject JNICALL Java_TestJNI_nativeNewObject2(JNIEnv* env, jobject thiz, jstring message, jint i, jdouble d, jfloat f, jlong l, jshort s, jbyte b, jchar c, jboolean bool_) {
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

JNIEXPORT jobject JNICALL Java_TestJNI_nativeNewObjectV(JNIEnv* env, jobject thiz, ...) {
    jclass cls = (*env)->FindClass(env, "NativeTestObject");
    if (cls == NULL) {
        (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/ClassNotFoundException"), "NativeTestObject not found");
        return NULL;
    }
    jmethodID ctor = (*env)->GetMethodID(env, cls, "<init>", "(Ljava/lang/String;IDFJSBCZ)V");
    // Initialize the va_list
    va_list args;
    va_start(args, thiz);
    jobject obj = (*env)->NewObjectV(env, cls, ctor, args);
    // Clean up
    va_end(args);
    (*env)->DeleteLocalRef(env, cls);
    if (obj == NULL) {
        (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/InstantiationException"), "Failed to create NativeTestObject");
    }
    return obj;
}

JNIEXPORT jobject JNICALL Java_TestJNI_nativeNewObjectA(JNIEnv* env, jobject thiz, jobjectArray args) {
    jclass cls = (*env)->FindClass(env, "NativeTestObject");
    if (cls == NULL) {
        (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/ClassNotFoundException"), "NativeTestObject not found");
        return NULL;
    }
    jmethodID ctor = (*env)->GetMethodID(env, cls, "<init>", "(Ljava/lang/String;IDFJSBCZ)V");
    // Convert the jobjectArray to a jvalue array
    jsize len = (*env)->GetArrayLength(env, args);
    jvalue* jargs = (jvalue*)malloc(len * sizeof(jvalue));
    if (jargs == NULL) {
        (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/OutOfMemoryError"), "Failed to allocate memory for jvalue array");
        (*env)->DeleteLocalRef(env, cls);
        return NULL;
    }

    // Set the arguments in the jvalue array
    jargs[0].l = (*env)->GetObjectArrayElement(env, args, 0);  // String (message)
    jargs[1].i = (*env)->CallIntMethod(env, (*env)->GetObjectArrayElement(env, args, 1), (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Integer"), "intValue", "()I"));  // int
    jargs[2].d = (*env)->CallDoubleMethod(env, (*env)->GetObjectArrayElement(env, args, 2), (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Double"), "doubleValue", "()D"));  // double
    jargs[3].f = (*env)->CallFloatMethod(env, (*env)->GetObjectArrayElement(env, args, 3), (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Float"), "floatValue", "()F"));  // float
    jargs[4].j = (*env)->CallLongMethod(env, (*env)->GetObjectArrayElement(env, args, 4), (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Long"), "longValue", "()J"));  // long
    jargs[5].s = (*env)->CallShortMethod(env, (*env)->GetObjectArrayElement(env, args, 5), (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Short"), "shortValue", "()S"));  // short
    jargs[6].b = (*env)->CallByteMethod(env, (*env)->GetObjectArrayElement(env, args, 6), (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Byte"), "byteValue", "()B"));  // byte
    jargs[7].c = (*env)->CallCharMethod(env, (*env)->GetObjectArrayElement(env, args, 7), (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Character"), "charValue", "()C"));  // char
    jargs[8].z = (*env)->CallBooleanMethod(env, (*env)->GetObjectArrayElement(env, args, 8), (*env)->GetMethodID(env, (*env)->FindClass(env, "java/lang/Boolean"), "booleanValue", "()Z"));  // boolean

    // Create a new instance of NativeTestObject
    jobject obj = (*env)->NewObjectA(env, cls, ctor, jargs);
    // Clean up
    free(jargs);
    (*env)->DeleteLocalRef(env, cls);
    if (obj == NULL) {
        (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/InstantiationException"), "Failed to create NativeTestObject");
    }
    return obj;
}

// ------------ Thread test -------------

void* thread_fn(void* arg) {
    JavaVM* jvm = (JavaVM*)arg;
    JNIEnv* env;

    if ((*jvm)->AttachCurrentThread(jvm, (void **)&env, NULL) != 0)
        return NULL;

    (*jvm)->DetachCurrentThread(jvm);
    return NULL;
}

JNIEXPORT jboolean JNICALL Java_TestJNI_nativeThreadTest(JNIEnv* env, jobject thiz) {
    JavaVM* jvm;
    (*env)->GetJavaVM(env, &jvm);

    pthread_t t;
    pthread_create(&t, NULL, thread_fn, jvm);
    pthread_join(t, NULL);

    return JNI_TRUE;
}
