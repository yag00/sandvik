#include "array.hpp"

#include <fmt/format.h>
#include <jni/jni.h>

#include "class.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "jnihandlemap.hpp"
#include "object.hpp"
#include "system/logger.hpp"

extern "C" {
	JNIEXPORT jobject JNICALL Java_java_lang_reflect_Array_multiNewArray(JNIEnv* env, jclass, jobject classObj, jobject dimsArr) {
		sandvik::NativeInterface* jenv = static_cast<sandvik::NativeInterface*>(env);
		if (jenv == nullptr) {
			throw std::runtime_error("Internal error: JNIEnv is not a NativeInterface");
		}
		if (classObj == nullptr || dimsArr == nullptr) {
			throw std::runtime_error("NullPointerException");
		}

		// Get the class type
		sandvik::Object* obj = (sandvik::Object*)classObj;
		sandvik::ConstClassObject* cls = static_cast<sandvik::ConstClassObject*>(obj);
		if (!cls) {
			throw std::runtime_error("Invalid class object");
		}
		logger.fdebug("Creating multi-dimensional array of type: {}", cls->getClassType().getFullname());

		// Get dimensions from dimsArr (assume it's a Java int array)
		std::vector<uint32_t> dimensions;
		sandvik::Array* dimArray = (sandvik::Array*)dimsArr;
		if (!dimArray) {
			throw std::runtime_error("Invalid dimensions array");
		}
		logger.fdebug("Array dimensions: {}", dimArray->getArrayLength());
		for (size_t i = 0; i < dimArray->getArrayLength(); ++i) {
			auto numObj = std::dynamic_pointer_cast<sandvik::NumberObject>(dimArray->getElement(i));
			if (!numObj) {
				throw std::runtime_error(fmt::format("Dimension at index {} is not a NumberObject", i));
			}
			logger.fdebug(" - dim[{}] = {}", i, numObj->getValue());
			dimensions.push_back(numObj->getValue());
		}

		// Create the multi-dimensional array
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		auto arrObj = sandvik::Object::makeArray(classloader, cls->getClassType(), dimensions);

		// Return as jobject
		return jenv->getHandles().toJObject(arrObj);
	}
}
