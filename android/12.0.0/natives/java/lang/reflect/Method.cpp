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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "method.hpp"

#include <fmt/format.h>
#include <jni/jni.h>

#include <map>

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "frame.hpp"
#include "jni.hpp"
#include "jthread.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "vm.hpp"

using namespace sandvik;

namespace {
	// Boxes a primitive return value into its java.lang wrapper class.
	ObjectRef boxPrimitive(ClassLoader& classloader_, char type_, uint64_t value_) {
		static const std::map<char, std::string, std::less<>> wrappers = {{'Z', "java.lang.Boolean"}, {'B', "java.lang.Byte"},    {'C', "java.lang.Character"},
		                                                                  {'S', "java.lang.Short"},   {'I', "java.lang.Integer"}, {'J', "java.lang.Long"},
		                                                                  {'F', "java.lang.Float"},   {'D', "java.lang.Double"}};
		auto it = wrappers.find(type_);
		if (it == wrappers.end()) {
			throw VmException("Method.invoke: unknown primitive return type '{}'", type_);
		}
		auto boxed = Object::make(classloader_.getOrLoad(it->second));
		boxed->setField("value", Object::make(value_));
		return boxed;
	}

	// Unboxes a java.lang wrapper object, as stored in its "value" field.
	ObjectRef unboxPrimitive(const ObjectRef& boxed_, const std::string& type_) {
		if (boxed_ == nullptr || boxed_->isNull()) {
			throw IllegalArgumentException(fmt::format("Method.invoke: null passed for primitive argument of type {}", type_));
		}
		return boxed_->getField("value");
	}
}  // namespace

extern "C" {

#if 0
JNIEXPORT void JNICALL Java_java_lang_reflect_Method_getDefaultValue(JNIEnv* env, jobject obj) {
    logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

	JNIEXPORT jobject JNICALL Java_java_lang_reflect_Method_invoke(JNIEnv* env, jobject obj, jobject receiver, jobjectArray args) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto& classloader = jenv->getClassLoader();
		auto methodObj = sandvik::native::getObject(obj);
		// artMethod holds the address of the sandvik::Method (see Class.getDeclaredMethodInternal)
		auto* method = reinterpret_cast<Method*>(static_cast<uintptr_t>(methodObj->getField("artMethod")->getLongValue()));
		if (method == nullptr) {
			throw VmException("Method.invoke: reflected method is not bound to a VM method");
		}

		auto receiverObj = sandvik::native::getObject(receiver);
		bool hasReceiver = receiverObj != nullptr && !receiverObj->isNull();
		if (!method->isStatic()) {
			if (!hasReceiver) {
				throw NullPointerException(fmt::format("Method.invoke: null receiver for {}.{}", method->getClass().getFullname(), method->getName()));
			}
			// Virtual dispatch on the receiver runtime class
			if (!method->isPrivate() && !method->isConstructor()) {
				method = &receiverObj->getClass().getMethod(method->getName(), method->getSignature());
			}
		}

		const auto& argTypes = method->arguments();
		auto* argsArray = sandvik::native::getArray(args);
		uint32_t nbArgs = (argsArray == nullptr || argsArray->isNull()) ? 0 : argsArray->getArrayLength();
		if (nbArgs != argTypes.size()) {
			throw IllegalArgumentException(fmt::format("Method.invoke: wrong number of arguments for {}.{}: expected {}, got {}",
			                                           method->getClass().getFullname(), method->getName(), argTypes.size(), nbArgs));
		}
		logger.ftrace("Method.invoke {}.{}{}", method->getClass().getFullname(), method->getName(), method->getSignature());

		auto& thread = jenv->getVm().newThread(fmt::format("{}.{}", method->getClass().getFullname(), method->getName()));
		auto& frame = thread.newFrame(*method);

		// Incoming arguments are placed in the last registers of the frame
		uint32_t paramWords = method->isStatic() ? 0u : 1u;
		for (const auto& type : argTypes) {
			paramWords += (type[0] == 'J' || type[0] == 'D') ? 2u : 1u;
		}
		if (paramWords > method->getNbRegisters()) {
			throw VmException("Method.invoke: method {} has invalid register layout", method->getName());
		}
		uint32_t regidx = method->getNbRegisters() - paramWords;
		if (!method->isStatic()) {
			frame.setObjRegister(regidx++, receiverObj);
		}
		for (uint32_t i = 0; i < nbArgs; ++i) {
			const auto& type = argTypes[i];
			auto arg = argsArray->getElement(i);
			switch (type[0]) {
				case 'L':
				case '[':
					frame.setObjRegister(regidx++, arg);
					break;
				case 'J':
				case 'D': {
					auto value = static_cast<uint64_t>(unboxPrimitive(arg, type)->getLongValue());
					frame.setObjRegister(regidx++, Object::make(static_cast<uint64_t>(static_cast<uint32_t>(value & 0xFFFFFFFF))));
					frame.setObjRegister(regidx++, Object::make(static_cast<uint64_t>(static_cast<uint32_t>((value >> 32) & 0xFFFFFFFF))));
					break;
				}
				default:
					frame.setObjRegister(regidx++, unboxPrimitive(arg, type));
					break;
			}
		}

		thread.run(true);

		const auto signature = method->getSignature();
		const char returnType = signature[signature.find(')') + 1];
		switch (returnType) {
			case 'V':
				return (jobject)Object::makeNull();
			case 'L':
			case '[':
				return (jobject)thread.getReturnObject();
			case 'J':
			case 'D':
				return (jobject)boxPrimitive(classloader, returnType, static_cast<uint64_t>(thread.getReturnDoubleValue()));
			default:
				return (jobject)boxPrimitive(classloader, returnType, static_cast<uint64_t>(static_cast<int64_t>(thread.getReturnValue())));
		}
	}

}  // extern "C"
