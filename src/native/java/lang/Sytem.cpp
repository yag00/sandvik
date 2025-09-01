#include <fmt/format.h>
#include <jni/jni.h>

#include "class.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "vm.hpp"

extern "C" {
	JNIEXPORT void JNICALL Java_java_lang_System_initializeStream(JNIEnv *env, jobject obj) {
		jclass systemClass = env->FindClass("java.lang.System");
		jclass printStreamClass = env->FindClass("java.io.PrintStream");

		auto *systemClassObj = reinterpret_cast<sandvik::Class *>(systemClass);
		// initialize System.out
		auto stream_stdout = sandvik::Object::make(*reinterpret_cast<sandvik::Class *>(printStreamClass));
		stream_stdout->setField("file", sandvik::Object::make(1));
		systemClassObj->getField("out").setObjectValue(stream_stdout);
		// initialize System.err
		auto stream_stderr = sandvik::Object::make(*reinterpret_cast<sandvik::Class *>(printStreamClass));
		stream_stderr->setField("file", sandvik::Object::make(2));
		systemClassObj->getField("err").setObjectValue(stream_stdout);
	}

	JNIEXPORT void JNICALL Java_java_lang_System_loadLibrary(JNIEnv *env, jobject obj, jstring libName) {
		sandvik::NativeInterface *jenv = static_cast<sandvik::NativeInterface *>(env);
		if (jenv == nullptr) {
			throw std::runtime_error("Internal error: JNIEnv is not a NativeInterface");
		}
		const char *name = env->GetStringUTFChars(libName, nullptr);
		jenv->getVm().loadLibrary(fmt::format("lib{}.so", name));
		env->ReleaseStringUTFChars(libName, name);
	}
}