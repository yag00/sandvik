#include "class.hpp"

#include <fmt/format.h>
#include <jni/jni.h>

#include "field.hpp"
#include "jni.hpp"
#include "jnihandlemap.hpp"
#include "object.hpp"
#include "system/logger.hpp"

extern "C" {
	JNIEXPORT jstring JNICALL Java_java_lang_Class_getName(JNIEnv* env, jobject obj) {
		sandvik::NativeInterface* jenv = static_cast<sandvik::NativeInterface*>(env);
		if (jenv == nullptr) {
			throw std::runtime_error("Internal error: JNIEnv is not a NativeInterface");
		}
		sandvik::Object* ptr = (sandvik::Object*)obj;
		if (ptr == nullptr) {
			throw std::runtime_error("NullPointerException");
		}
		sandvik::ConstClassObject* classObj = dynamic_cast<sandvik::ConstClassObject*>(ptr);
		std::string name = classObj->getClassType().getFullname();
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		auto strObj = sandvik::Object::make(classloader, name);
		jobject jstr = jenv->getHandles().toJObject(strObj);
		return (jstring)jstr;
	}
}