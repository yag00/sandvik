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

#include <fmt/format.h>
#include <jni/jni.h>

#include "array.hpp"
#include "class.hpp"
#include "exceptions.hpp"
#include "frame.hpp"
#include "jni.hpp"
#include "jthread.hpp"
#include "method.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "vm.hpp"

using namespace sandvik;

extern "C" {

	JNIEXPORT jobject JNICALL Java_java_lang_reflect_Constructor_newInstance0(JNIEnv* env, jobject obj, jobjectArray args) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto constructorObj = sandvik::native::getObject(obj);
		auto* constructor = reinterpret_cast<Method*>(static_cast<uintptr_t>(constructorObj->getField("artMethod")->getLongValue()));
		if (constructor == nullptr || !constructor->isConstructor()) {
			throw VmException("Constructor.newInstance0: reflected constructor is not bound to a VM constructor");
		}

		auto* argsArray = sandvik::native::getArray(args);
		uint32_t nbArgs = (argsArray == nullptr || argsArray->isNull()) ? 0 : argsArray->getArrayLength();
		const auto& argumentTypes = constructor->arguments();
		if (nbArgs != argumentTypes.size()) {
			throw IllegalArgumentException(fmt::format("Constructor.newInstance0: wrong number of arguments for {}{}: expected {}, got {}",
			                                           constructor->getClass().getFullname(), constructor->getSignature(), argumentTypes.size(), nbArgs));
		}

		auto instance = Object::make(constructor->getClass());
		auto& thread = jenv->getVm().newThread(fmt::format("{}.{}", constructor->getClass().getFullname(), constructor->getName()));
		auto& frame = thread.newFrame(*constructor);

		uint32_t parameterWords = 1;
		for (const auto& type : argumentTypes) {
			parameterWords += (type[0] == 'J' || type[0] == 'D') ? 2u : 1u;
		}
		if (parameterWords > constructor->getNbRegisters()) {
			throw VmException("Constructor.newInstance0: constructor has invalid register layout");
		}
		uint32_t registerIndex = constructor->getNbRegisters() - parameterWords;
		frame.setObjRegister(registerIndex++, instance);

		for (uint32_t i = 0; i < nbArgs; ++i) {
			const auto& type = argumentTypes[i];
			auto argument = argsArray->getElement(i);
			switch (type[0]) {
				case 'L':
				case '[':
					frame.setObjRegister(registerIndex++, argument);
					break;
				case 'J':
				case 'D': {
					if (argument == nullptr || argument->isNull()) {
						throw IllegalArgumentException("Constructor.newInstance0: null passed for primitive argument");
					}
					auto value = static_cast<uint64_t>(argument->getField("value")->getLongValue());
					frame.setObjRegister(registerIndex++, Object::make(static_cast<uint32_t>(value & 0xFFFFFFFF)));
					frame.setObjRegister(registerIndex++, Object::make(static_cast<uint32_t>(value >> 32)));
					break;
				}
				default:
					if (argument == nullptr || argument->isNull()) {
						throw IllegalArgumentException("Constructor.newInstance0: null passed for primitive argument");
					}
					frame.setObjRegister(registerIndex++, argument->getField("value"));
					break;
			}
		}

		thread.run(true);
		return (jobject)instance;
	}

}  // extern "C"
