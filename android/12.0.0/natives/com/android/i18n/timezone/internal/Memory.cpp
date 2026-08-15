#include <jni/jni.h>

#include "array.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"

using namespace sandvik;

extern "C" {

	JNIEXPORT void JNICALL Java_com_android_i18n_timezone_internal_Memory_peekByteArray(JNIEnv* env, jclass, jlong address, jbyteArray dest, jint offset,
	                                                                                    jint length) {
		if (dest == nullptr) {
			throw NullPointerException("array is null");
		}
		if (offset < 0 || length < 0 || static_cast<size_t>(offset + length) > static_cast<size_t>(env->GetArrayLength(dest))) {
			throw IndexOutOfBoundsException("Invalid offset or length for the given array");
		}
		const jbyte* srcPtr = reinterpret_cast<const jbyte*>(static_cast<uintptr_t>(address));
		env->SetByteArrayRegion(dest, offset, length, srcPtr);
	}

}  // extern "C"