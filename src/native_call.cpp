/*
 * This file is part of Sandvik project.
 * Copyright (C) 2025 Christophe Duvernois
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "native_call.hpp"

#include <fmt/format.h>

#include <cstring>
#include <variant>

#include "jni.hpp"
#include "jnihandlemap.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

NativeCallHelper::NativeCallHelper(NativeInterface& nif) : _nif(nif) {
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
			// Skip all leading '[' for multi-dimensional arrays
			while (i < paramTypes.size() && paramTypes[i] == '[') i++;
			// If it's an object array, skip until the end of the object descriptor
			if (i < paramTypes.size() && paramTypes[i] == 'L') {
				while (i < paramTypes.size() && paramTypes[i] != ';') i++;
				if (i < paramTypes.size()) i++;  // skip semicolon
			} else {
				// For primitive arrays, just skip the primitive type character
				if (i < paramTypes.size()) i++;
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

uintptr_t NativeCallHelper::getArgValue(std::vector<std::shared_ptr<Object>>::iterator& it, const char jniType) {
	switch (jniType) {
		case 'I':
		case 'Z':
		case 'B':
		case 'S':
		case 'C': {
			auto number = std::dynamic_pointer_cast<NumberObject>(*it);
			++it;
			if (!number) {
				throw std::runtime_error(fmt::format("Invalid argument type for JNI type: {}", jniType));
			}
			return static_cast<uintptr_t>(number->getValue());
		}
		case 'J': {
			auto lsb = std::dynamic_pointer_cast<NumberObject>(*it);
			++it;
			auto msb = std::dynamic_pointer_cast<NumberObject>(*it);
			++it;
			if (!lsb || !msb) {
				throw std::runtime_error(fmt::format("Invalid argument type for JNI type: {}", jniType));
			}
			uint32_t lsb_value = lsb->getValue();
			uint32_t msb_value = msb->getValue();
			uintptr_t result = (static_cast<uintptr_t>(msb_value) << 32) | static_cast<uintptr_t>(lsb_value);
			return result;
		}
		case 'F': {
			auto number = std::dynamic_pointer_cast<NumberObject>(*it);
			++it;
			if (!number) {
				throw std::runtime_error(fmt::format("Invalid argument type for JNI type: {}", jniType));
			}
			uintptr_t result;
			float value = number->getFloatValue();
			std::memcpy(&result, &value, sizeof(value));
			return result;
		}
		case 'D': {
			auto lsb = std::dynamic_pointer_cast<NumberObject>(*it);
			++it;
			auto msb = std::dynamic_pointer_cast<NumberObject>(*it);
			++it;
			if (!lsb || !msb) {
				throw std::runtime_error(fmt::format("Invalid argument type for JNI type: {}", jniType));
			}
			uint32_t lsb_value = lsb->getValue();
			uint32_t msb_value = msb->getValue();
			uintptr_t result = (static_cast<uintptr_t>(msb_value) << 32) | static_cast<uintptr_t>(lsb_value);
			return result;
		}
		case 'L':
		case '[':  // Objects and arrays
		{
			auto obj = *it;
			++it;
			jobject o = _nif.getHandles().toJObject(obj);
			_handles.push_back((uintptr_t)o);
			return (uintptr_t)o;
		}
		default:
			throw std::runtime_error(fmt::format("Unsupported JNI type character: {}", jniType));
	}
}

std::shared_ptr<Object> NativeCallHelper::getReturnObject(uintptr_t result, const char jniType) {
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
		case '[': {  // Objects and arrays
			auto ret = _nif.getHandles().fromJObject((jobject)result);
			if (ret == nullptr) {
				return Object::makeNull();
			}
			_handles.push_back(result);
			return ret;
		}
		default:
			throw std::runtime_error(fmt::format("Unsupported JNI type character: {}", jniType));
	}
}

std::shared_ptr<Object> NativeCallHelper::invoke(void* functionPtr, JNIEnv* env, const std::vector<std::shared_ptr<Object>>& args,
                                                 const std::string& returnType, const std::string& paramTypes, bool isStatic) {
	_handles.clear();
	// Create a temporary call context
	std::vector<std::string> argTypes;
	CallContext context;
	prepareCallContext(context, paramTypes, returnType, argTypes);

	// Execute the call
	uintptr_t result_storage = 0;
	uintptr_t* param_storage = 0;
	if (args.size() > 0) {
		param_storage = new uintptr_t[args.size()];
	}
	std::vector<void*> arg_values;
	// jni environment pointer
	arg_values.push_back(&env);
	// jobject this reference
	jobject this_ref;
	if (isStatic) {
		// static method
		this_ref = nullptr;  // todo handle this case, should be a class object reference
	} else {
		this_ref = _nif.getHandles().toJObject(args[0]);
		_handles.push_back((uintptr_t)this_ref);
	}
	arg_values.push_back(&this_ref);
	// Prepare argument values
	size_t idx = 0;
	auto mutableArgs = args;  // Create a mutable copy of args
	auto it = mutableArgs.begin();
	if (!isStatic) {
		// If not static, skip the first argument (this reference)
		it++;
	}
	while (it != mutableArgs.end()) {
		param_storage[idx] = getArgValue(it, argTypes[idx][0]);
		arg_values.push_back((void*)&param_storage[idx]);
		idx++;
	}

	ffi_call(&context.cif, FFI_FN(functionPtr), &result_storage, arg_values.data());

	// Process and return result
	if (param_storage) {
		delete[] param_storage;
	}
	auto ret = getReturnObject(result_storage, returnType[0]);
	for (auto handle : _handles) {
		_nif.getHandles().release((jobject)handle);
	}
	_handles.clear();
	return ret;
}
