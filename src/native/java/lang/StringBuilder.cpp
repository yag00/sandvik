#include <fmt/format.h>
#include <jni/jni.h>

#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "jnihandlemap.hpp"
#include "object.hpp"
#include "system/logger.hpp"

extern "C" {
	JNIEXPORT jobject JNICALL Java_java_lang_StringBuilder_append__Ljava_lang_String_2(JNIEnv* env, jobject obj, jstring str) {
		sandvik::Object* this_ptr = (sandvik::Object*)obj;
		if (this_ptr == nullptr) {
			throw sandvik::NullPointerException("Append on null StringBuilder object");
		}
		sandvik::NativeInterface* jenv = static_cast<sandvik::NativeInterface*>(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();

		const char* nativeStr = env->GetStringUTFChars(str, nullptr);

		if (this_ptr->getField("string")->isNull()) {
			this_ptr->setField("string", sandvik::Object::make(classloader, std::string(nativeStr)));
		} else {
			auto current = this_ptr->getField("string");
			if (!current->isString()) {
				throw sandvik::ClassCastException("Field 'string' is not a StringObject");
			}
			this_ptr->setField("string", sandvik::Object::make(classloader, current->str() + std::string(nativeStr)));
		}
		return obj;
	}

	JNIEXPORT jobject JNICALL Java_java_lang_StringBuilder_append__I(JNIEnv* env, jobject obj, jint i) {
		sandvik::Object* this_ptr = (sandvik::Object*)obj;
		if (this_ptr == nullptr) {
			throw sandvik::NullPointerException("Append on null StringBuilder object");
		}
		sandvik::NativeInterface* jenv = static_cast<sandvik::NativeInterface*>(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();

		if (this_ptr->getField("string")->isNull()) {
			this_ptr->setField("string", sandvik::Object::make(classloader, std::to_string(i)));
		} else {
			auto current = this_ptr->getField("string");
			if (!current->isString()) {
				throw sandvik::ClassCastException("Field 'string' is not a StringObject");
			}
			this_ptr->setField("string", sandvik::Object::make(classloader, current->str() + std::to_string(i)));
		}
		return obj;
	}
	JNIEXPORT jobject JNICALL Java_java_lang_StringBuilder_append__F(JNIEnv* env, jobject obj, jfloat f) {
		sandvik::Object* this_ptr = (sandvik::Object*)obj;
		if (this_ptr == nullptr) {
			throw sandvik::NullPointerException("Append on null StringBuilder object");
		}
		sandvik::NativeInterface* jenv = static_cast<sandvik::NativeInterface*>(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();

		std::string floatStr = std::to_string(f);

		if (this_ptr->getField("string")->isNull()) {
			this_ptr->setField("string", sandvik::Object::make(classloader, floatStr));
		} else {
			auto current = this_ptr->getField("string");
			if (!current->isString()) {
				throw sandvik::ClassCastException("Field 'string' is not a StringObject");
			}
			this_ptr->setField("string", sandvik::Object::make(classloader, current->str() + floatStr));
		}
		return obj;
	}
	JNIEXPORT jobject JNICALL Java_java_lang_StringBuilder_append__D(JNIEnv* env, jobject obj, jdouble d) {
		sandvik::Object* this_ptr = (sandvik::Object*)obj;
		if (this_ptr == nullptr) {
			throw sandvik::NullPointerException("Append on null StringBuilder object");
		}
		sandvik::NativeInterface* jenv = static_cast<sandvik::NativeInterface*>(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();

		std::string doubleStr = std::to_string(d);

		if (this_ptr->getField("string")->isNull()) {
			this_ptr->setField("string", sandvik::Object::make(classloader, doubleStr));
		} else {
			auto current = this_ptr->getField("string");
			if (!current->isString()) {
				throw sandvik::ClassCastException("Field 'string' is not a StringObject");
			}
			this_ptr->setField("string", sandvik::Object::make(classloader, current->str() + doubleStr));
		}
		return obj;
	}

	JNIEXPORT jobject JNICALL Java_java_lang_StringBuilder_append__J(JNIEnv* env, jobject obj, jlong j) {
		sandvik::Object* this_ptr = (sandvik::Object*)obj;
		if (this_ptr == nullptr) {
			throw sandvik::NullPointerException("Append on null StringBuilder object");
		}
		sandvik::NativeInterface* jenv = static_cast<sandvik::NativeInterface*>(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();

		std::string longStr = std::to_string(j);

		if (this_ptr->getField("string")->isNull()) {
			this_ptr->setField("string", sandvik::Object::make(classloader, longStr));
		} else {
			auto current = this_ptr->getField("string");
			if (!current->isString()) {
				throw sandvik::ClassCastException("Field 'string' is not a StringObject");
			}
			this_ptr->setField("string", sandvik::Object::make(classloader, current->str() + longStr));
		}
		return obj;
	}

	JNIEXPORT jobject JNICALL Java_java_lang_StringBuilder_toString(JNIEnv* env, jobject obj) {
		sandvik::NativeInterface* jenv = static_cast<sandvik::NativeInterface*>(env);
		if (jenv == nullptr) {
			throw std::runtime_error("Internal error: JNIEnv is not a NativeInterface");
		}
		sandvik::Object* this_ptr = (sandvik::Object*)obj;
		if (this_ptr == nullptr) {
			throw sandvik::NullPointerException("Append on null StringBuilder object");
		}
		sandvik::ClassLoader& classloader = jenv->getClassLoader();

		if (this_ptr->getField("string")->isNull()) {
			throw sandvik::NullPointerException("Field 'string' is null");
		}
		auto current = this_ptr->getField("string");
		if (!current->isString()) {
			throw sandvik::ClassCastException("Field 'string' is not a StringObject");
		}
		auto strObj = sandvik::Object::make(classloader, current->str());
		jobject jstr = jenv->getHandles().toJObject(strObj);
		return jstr;
	}
}