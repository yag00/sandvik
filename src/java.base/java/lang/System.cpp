
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
	class System {
		public:
	};
}  // namespace

namespace java {
	namespace lang {
		void System(::sandvik::ClassLoader& classLoader) {
			ClassBuilder builder(classLoader, "java.lang", "java.lang.System");
			// @todo fix the static initialization of System class here we have dependency on PrintStream
			::sandvik::Class& printStreamClass = classLoader.getOrLoad("java.io.PrintStream");
			builder.addField("out", "Ljava/io/PrintStream;", true, Object::make(printStreamClass));
			builder.finalize();
		}
	}  // namespace lang
}  // namespace java