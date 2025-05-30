
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
	class Package {
		public:
			static void getImplementationVersion(ClassLoader& classloader_, Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 1) {
					throw std::runtime_error("Not enough arguments for getName");
				}
				auto clazz = std::dynamic_pointer_cast<ObjectClass>(args_[0]);
				if (!clazz || !clazz->isInstanceOf("java.lang.Package")) {
					throw std::runtime_error("First argument is not an instance of java.lang.Package");
				}
				frame_.setReturnObject(Object::make(classloader_, "Sandvik VM"));
			}
	};
}  // namespace

namespace java {
	namespace lang {
		void Package(::sandvik::ClassLoader& classLoader) {
			ClassBuilder builder(classLoader, "java.lang", "java.lang.Package");
			builder.addVirtualMethod("getImplementationVersion", "()Ljava/lang/String;", 0,
			                         [&classLoader](Frame& frame_, std::vector<std::shared_ptr<::sandvik::Object>>& args_) {
				                         Package::getImplementationVersion(classLoader, frame_, args_);
			                         });
			builder.finalize();
		}
	}  // namespace lang
}  // namespace java