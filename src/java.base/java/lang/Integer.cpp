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
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

namespace {
	class Integer {
		public:
			static void parseInt(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 1) {
					throw std::runtime_error("Not enough arguments for isNaN");
				}
				auto value = std::dynamic_pointer_cast<StringObject>(args_[0]);
				if (!value) {
					throw std::runtime_error("Arguments must an instance of java.lang.String");
				}
				auto str = value->str();
				int result;
				if (!str.empty() && str[0] == '#') {
					result = std::stoi(str.substr(1), nullptr, 16);
				} else {
					result = std::stoi(str, nullptr, 10);
				}
				frame_.setReturnValue(result);
			}
	};
}  // namespace

namespace java {
	namespace lang {
		void Integer(::sandvik::ClassLoader& classLoader) {
			ClassBuilder builder(classLoader, "java.lang", "java.lang.Integer");
			builder.addMethod("parseInt", "(Ljava/lang/String;)I", ACCESS_FLAGS::ACC_STATIC, Integer::parseInt);
			builder.finalize();
		}
	}  // namespace lang
}  // namespace java