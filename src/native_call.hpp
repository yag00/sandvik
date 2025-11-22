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

#ifndef __NATIVE_CALL_HELPER_HPP__
#define __NATIVE_CALL_HELPER_HPP__

#include <ffi.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "jni/jni.h"

#include "object.hpp"

namespace sandvik {
	class NativeInterface;
	/** @brief Helper class to invoke native functions */
	class NativeCallHelper {
		public:
			/** Constructor for NativeCallHelper.
			 * @param nif Reference to the NativeInterface. */
			explicit NativeCallHelper(NativeInterface& nif);
			~NativeCallHelper();

			/** @brief method to invoke native functions
			 * @param functionPtr Pointer to the native function to invoke
			 * @param env Pointer to the JNI environment
			 * @param args Vector of Object references representing the arguments
			 * @param paramTypes String representing the parameter types in JNI format
			 * @param isStatic Boolean indicating if the method is static
			 * @param returnType String representing the return type in JNI format */
			ObjectRef invoke(void* functionPtr, JNIEnv* env, const std::vector<ObjectRef>& args, const std::string& returnType, const std::string& paramTypes,
			                 bool isStatic = false);

		private:
			NativeInterface& _nif;
			// Internal call context
			struct CallContext {
					ffi_cif cif;
					std::vector<ffi_type*> arg_types;
					std::string return_type;
					bool prepared = false;
			};

			// Type conversion helpers
			ffi_type* getFFITypeForJNIType(char jniType) const;
			ffi_type* getFFITypeForReturn(const std::string& returnType) const;
			void prepareCallContext(CallContext& context, const std::string& paramTypes, const std::string& returnType, std::vector<std::string>& argTypes);
			uintptr_t getArgValue(std::vector<ObjectRef>::iterator& it, const char jniType);
			ObjectRef getReturnObject(uintptr_t result, const char jniType);

			std::vector<uintptr_t> _handles;
	};
}  // namespace sandvik

#endif  // __NATIVE_CALL_HELPER_HPP__