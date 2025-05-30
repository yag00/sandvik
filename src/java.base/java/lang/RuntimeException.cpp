
#include <fmt/core.h>

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "class.hpp"
#include "classbuilder.hpp"
#include "classloader.hpp"
#include "field.hpp"
#include "frame.hpp"
#include "java.base/classes.hpp"
#include "method.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

namespace {
	class RuntimeException {
		public:
			static void init(ClassLoader& classloader_, Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 2) {
					throw std::runtime_error("Not enough arguments for RuntimeException.<init>");
				}
				auto clazz = std::dynamic_pointer_cast<ObjectClass>(args_[0]);
				if (!clazz || !clazz->isInstanceOf("java.lang.RuntimeException")) {
					throw std::runtime_error("First argument is not an instance of java.lang.RuntimeException");
				}
				auto message = std::dynamic_pointer_cast<StringObject>(args_[1]);
				if (!message) {
					throw std::runtime_error("Second argument is not an instance of java.lang.String");
				}
				clazz->setObjectData(std::make_shared<std::string>(message->str()));
			}
			static void getMessage(ClassLoader& classloader_, Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 1) {
					throw std::runtime_error("Not enough arguments for RuntimeException.getMessage");
				}
				auto clazz = std::dynamic_pointer_cast<ObjectClass>(args_[0]);
				if (!clazz || !clazz->isInstanceOf("java.lang.RuntimeException")) {
					throw std::runtime_error("First argument is not an instance of java.lang.RuntimeException");
				}
				auto message = clazz->getObjectData<std::string>();
				if (!message) {
					throw std::runtime_error("No message set for RuntimeException");
				}
				frame_.setReturnObject(Object::make(classloader_, *message));
			}
	};
}  // namespace

namespace java {
	namespace lang {
		void RuntimeException(::sandvik::ClassLoader& classLoader) {
			ClassBuilder builder(classLoader, "java.lang", "java.lang.RuntimeException");
			builder.addVirtualMethod(
			    "<init>", "(Ljava/lang/String;)V", 0,
			    [&classLoader](Frame& frame_, std::vector<std::shared_ptr<::sandvik::Object>>& args_) { RuntimeException::init(classLoader, frame_, args_); });
			builder.addVirtualMethod("getMessage", "()Ljava/lang/String;", 0,
			                         [&classLoader](Frame& frame_, std::vector<std::shared_ptr<::sandvik::Object>>& args_) {
				                         RuntimeException::getMessage(classLoader, frame_, args_);
			                         });
			builder.finalize();
		}
	}  // namespace lang
}  // namespace java