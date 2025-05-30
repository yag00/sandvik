
#include "object.hpp"

#include <fmt/format.h>

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
#include "system/logger.hpp"

using namespace sandvik;

namespace {
	class Object {
		public:
	};
}  // namespace

namespace java {
	namespace lang {
		void Object(::sandvik::ClassLoader& classLoader) {
			ClassBuilder builder(classLoader, "java.lang", "java.lang.Object");
			builder.addVirtualMethod("<init>", "()V", 0, [](Frame&, std::vector<std::shared_ptr<::sandvik::Object>>&) {});
			builder.finalize();
		}
	}  // namespace lang
}  // namespace java