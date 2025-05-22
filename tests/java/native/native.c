// gcc -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/linux" -shared -o libnative.so native.c
#include <jni.h>
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
