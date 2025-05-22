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
			                               const std::string& paramTypes);

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
			uintptr_t getArgValue(const std::shared_ptr<Object>& arg, const char jniType) const;
			std::shared_ptr<Object> getReturnObject(uintptr_t result, const char jniType) const;
	};
}  // namespace sandvik

#endif  // __NATIVE_CALL_HELPER_HPP__