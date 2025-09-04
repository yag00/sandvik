// gcc -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/linux" -shared -o libnative.so native.c
#include <jni.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    printf("JNI_OnLoad called: Native library loaded.\n");
    return JNI_VERSION_1_6; // Specify the JNI version your library supports
}

JNIEXPORT jint JNICALL Java_Native_test(
    JNIEnv *env,        // JNI environment pointer
    jobject obj,        // Java object this is attached to
    jstring jstr,       // Java String parameter
    jint a,             // First integer parameter
    jint b              // Second integer parameter
) {
    // Convert Java String to C string
    const char *str = (*env)->GetStringUTFChars(env, jstr, NULL);
    if (str == NULL) {
        return -1; // Error handling
    }

    // Print the received values (for demonstration)
    printf("Native function called with:\n");
    printf("  String: %s\n", str);
    printf("  Int a: %d\n", a);
    printf("  Int b: %d\n", b);

    // Example calculation - you can replace this with your actual logic
    jint result = a + b;

    // Release the string memory
    (*env)->ReleaseStringUTFChars(env, jstr, str);

    return result;
}
JNIEXPORT jdouble JNICALL Java_Native_doubleadd(
    JNIEnv *env,        // JNI environment pointer
    jobject obj,        // Java object this is attached to
    jdouble a,             // First integer parameter
    jdouble b              // Second integer parameter
) {
    printf("Native doubleadd called with:\n");
    printf("  Double a: 0x%lx %f\n", *(uint64_t*)&a, a);
    printf("  Double b: 0x%lx %f\n", *(uint64_t*)&b, b);
    jdouble result = a + b;
    printf("  Double r: 0x%lx %f\n", *(uint64_t*)&result, result);
    return result;
}
JNIEXPORT jlong JNICALL Java_Native_longadd(
    JNIEnv *env,        // JNI environment pointer
    jobject obj,        // Java object this is attached to
    jlong a,             // First integer parameter
    jlong b              // Second integer parameter
) {
    jlong result = a + b;
    return result;
}
JNIEXPORT jfloat JNICALL Java_Native_floatadd(
    JNIEnv *env,        // JNI environment pointer
    jobject obj,        // Java object this is attached to
    jfloat a,             // First integer parameter
    jfloat b              // Second integer parameter
) {
    printf("Native floatadd called with:\n");
    printf("  Float a: 0x%x %f\n", *(uint32_t*)&a, a);
    printf("  Float b: 0x%x %f\n", *(uint32_t*)&b, b);
    jfloat result = a + b;
    printf("  Float r: 0x%x %f\n", *(uint32_t*)&result, result);
    return result;
}