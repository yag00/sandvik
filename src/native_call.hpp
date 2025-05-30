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

namespace sandvik {
	class Object;
	class NativeCallHelper {
		public:
			// Constructor/Destructor
			NativeCallHelper();
			~NativeCallHelper();

			// Main method to invoke native functions
			std::shared_ptr<Object> invoke(void* functionPtr, JNIEnv* env, const std::vector<std::shared_ptr<Object>>& args, const std::string& returnType,
			                               const std::string& paramTypes, bool isStatic = false);

		private:
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
			uintptr_t getArgValue(std::vector<std::shared_ptr<Object>>::iterator& it, const char jniType) const;
			std::shared_ptr<Object> getReturnObject(uintptr_t result, const char jniType) const;
	};
}  // namespace sandvik

#endif  // __NATIVE_CALL_HELPER_HPP__