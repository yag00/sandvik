#include "native_call.hpp"

#include <fmt/format.h>

#include <cstring>
#include <variant>

#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

NativeCallHelper::NativeCallHelper() {
}

NativeCallHelper::~NativeCallHelper() {
}

ffi_type* NativeCallHelper::getFFITypeForJNIType(char jniType) const {
	switch (jniType) {
		case 'I':
		case 'Z':
		case 'B':
		case 'S':
		case 'C':
			return &ffi_type_sint32;
		case 'J':
			return &ffi_type_sint64;
		case 'F':
			return &ffi_type_float;
		case 'D':
			return &ffi_type_double;
		case 'L':
		case '[':  // Objects and arrays
			return &ffi_type_pointer;
		default:
			throw std::runtime_error(fmt::format("Unsupported JNI type character: {}", jniType));
	}
}

ffi_type* NativeCallHelper::getFFITypeForReturn(const std::string& returnType) const {
	if (returnType.empty() || returnType == "V") {
		return &ffi_type_void;
	}
	return getFFITypeForJNIType(returnType[0]);
}

void NativeCallHelper::prepareCallContext(CallContext& context, const std::string& paramTypes, const std::string& returnType,
                                          std::vector<std::string>& argTypes) {
	context.return_type = returnType;
	context.arg_types.clear();

	// Always reserve first two spots for JNIEnv* and jobject
	context.arg_types.push_back(&ffi_type_pointer);  // JNIEnv*
	context.arg_types.push_back(&ffi_type_pointer);  // jobject

	// Process parameter types
	for (size_t i = 0; i < paramTypes.size();) {
		char typeChar = paramTypes[i];
		// Skip opening parenthesis if present
		if (typeChar == '(' && i == 0) {
			i++;
			continue;
		}
		// Skip closing parenthesis if present
		if (typeChar == ')') {
			break;
		}
		argTypes.push_back(std::string(1, typeChar));
		context.arg_types.push_back(getFFITypeForJNIType(typeChar));

		// Handle type descriptors that take more than one character
		if (typeChar == 'L') {
			while (i < paramTypes.size() && paramTypes[i] != ';') i++;
			if (i < paramTypes.size()) i++;  // skip semicolon
		} else if (typeChar == '[') {
			while (i < paramTypes.size() && paramTypes[i] == '[') i++;
			if (i < paramTypes.size() && paramTypes[i] == 'L') {
				while (i < paramTypes.size() && paramTypes[i] != ';') i++;
				if (i < paramTypes.size()) i++;  // skip semicolon
			}
		} else {
			i++;  // simple type
		}
	}

	// Prepare the call interface
	if (ffi_prep_cif(&context.cif, FFI_DEFAULT_ABI, context.arg_types.size(), getFFITypeForReturn(returnType), context.arg_types.data()) != FFI_OK) {
		throw std::runtime_error("Failed to prepare FFI call interface");
	}
	context.prepared = true;
}

uintptr_t NativeCallHelper::getArgValue(const std::shared_ptr<Object>& arg, const char jniType) const {
	switch (jniType) {
		case 'I':
		case 'Z':
		case 'B':
		case 'S':
		case 'C': {
			auto number = std::dynamic_pointer_cast<NumberObject>(arg);
			if (!number) {
				throw std::runtime_error(fmt::format("Invalid argument type for JNI type: {}", jniType));
			}
			return static_cast<uintptr_t>(number->getValue());
		}
		case 'J': {
			auto number = std::dynamic_pointer_cast<NumberObject>(arg);
			if (!number) {
				throw std::runtime_error(fmt::format("Invalid argument type for JNI type: {}", jniType));
			}
			return static_cast<uintptr_t>(number->getLongValue());
		}
		case 'F': {
			auto number = std::dynamic_pointer_cast<NumberObject>(arg);
			if (!number) {
				throw std::runtime_error(fmt::format("Invalid argument type for JNI type: {}", jniType));
			}
			uintptr_t result;
			float value = number->getFloatValue();
			std::memcpy(&result, &value, sizeof(value));
			return result;
		}
		case 'D': {
			auto number = std::dynamic_pointer_cast<NumberObject>(arg);
			if (!number) {
				throw std::runtime_error(fmt::format("Invalid argument type for JNI type: {}", jniType));
			}
			uintptr_t result;
			double value = number->getDoubleValue();
			std::memcpy(&result, &value, sizeof(value));
			return result;
		}
		case 'L':
		case '[':  // Objects and arrays
			return (uintptr_t)arg.get();
		default:
			throw std::runtime_error(fmt::format("Unsupported JNI type character: {}", jniType));
	}
}

std::shared_ptr<Object> NativeCallHelper::getReturnObject(uintptr_t result, const char jniType) const {
	switch (jniType) {
		case 'V':
			return nullptr;  // void return type
		case 'I':
		case 'Z':
		case 'B':
		case 'S':
		case 'C':
		case 'J':
		case 'F':
		case 'D':
			return Object::make((uint64_t)result);
		case 'L':
		case '[':  // Objects and arrays
		default:
			throw std::runtime_error(fmt::format("Unsupported JNI type character: {}", jniType));
	}
}

std::shared_ptr<Object> NativeCallHelper::invoke(void* functionPtr, JNIEnv* env, const std::vector<std::shared_ptr<Object>>& args,
                                                 const std::string& returnType, const std::string& paramTypes) {
	// Create a temporary call context
	std::vector<std::string> argTypes;
	CallContext context;
	prepareCallContext(context, paramTypes, returnType, argTypes);
	// Prepare the arguments
	uintptr_t* args_array = new uintptr_t[args.size()];
	for (size_t i = 1; i < args.size(); i++) {
		args_array[i - 1] = getArgValue(args[i], argTypes[i - 1][0]);
	}
	// Execute the call
	uintptr_t result_storage = 0;
	std::vector<void*> arg_values;
	arg_values.push_back(&env);
	jobject this_ref = (jobject)(uintptr_t)(args[0].get());
	arg_values.push_back(&this_ref);
	for (uint32_t i = 0; i < args.size(); i++) {
		arg_values.push_back((void*)&args_array[i]);
	}
	ffi_call(&context.cif, FFI_FN(functionPtr), &result_storage, arg_values.data());
	// Process and return result
	delete[] args_array;
	return getReturnObject(result_storage, returnType[0]);
}
