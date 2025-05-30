
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
#include "vm.hpp"

using namespace sandvik;

namespace {
	class System {
		public:
			void static loadLibrary(Vm& vm_, Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 1) {
					throw std::runtime_error("Not enough arguments for loadLibrary");
				}
				auto lib = std::dynamic_pointer_cast<StringObject>(args_[0]);
				if (!lib) {
					throw std::runtime_error("Argument must be an instance of java.lang.String");
				}
				auto libName = fmt::format("lib{}.so", lib->str());
				logger.info(fmt::format("Loading library: {}", libName));
				vm_.loadLibrary(libName);
			}
	};
}  // namespace

namespace java {
	namespace lang {
		void System(::sandvik::Vm& vm, ::sandvik::ClassLoader& classLoader) {
			ClassBuilder builder(classLoader, "java.lang", "java.lang.System");
			// @todo fix the static initialization of System class here we have dependency on PrintStream
			::sandvik::Class& printStreamClass = classLoader.getOrLoad("java.io.PrintStream");
			builder.addField("out", "Ljava/io/PrintStream;", true, Object::make(printStreamClass));
			builder.addMethod("loadLibrary", "(Ljava/lang/String;)V", ACCESS_FLAGS::ACC_STATIC,
			                  [&vm](Frame& frame_, std::vector<std::shared_ptr<::sandvik::Object>>& args_) { System::loadLibrary(vm, frame_, args_); });
			builder.finalize();
		}
	}  // namespace lang
}  // namespace java