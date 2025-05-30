
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
	class Math {
		public:
			static void abs(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 1) {
					throw std::runtime_error("Not enough arguments for isNaN");
				}
				auto value = std::dynamic_pointer_cast<NumberObject>(args_[0]);
				if (!value) {
					throw std::runtime_error("Arguments must be numbers");
				}
				frame_.setReturnValue(std::abs(value->getValue()));
			}
			static void max(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 2) {
					throw std::runtime_error("Not enough arguments for max");
				}
				auto value1 = std::dynamic_pointer_cast<NumberObject>(args_[0]);
				auto value2 = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				if (!value1 || !value2) {
					throw std::runtime_error("Arguments must be numbers");
				}
				frame_.setReturnValue(std::max(value1->getValue(), value2->getValue()));
			}
			static void min(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 2) {
					throw std::runtime_error("Not enough arguments for min");
				}
				auto value1 = std::dynamic_pointer_cast<NumberObject>(args_[0]);
				auto value2 = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				if (!value1 || !value2) {
					throw std::runtime_error("Arguments must be numbers");
				}
				frame_.setReturnValue(std::min(value1->getValue(), value2->getValue()));
			}
			static void sqrt(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 1) {
					throw std::runtime_error("Not enough arguments for sqrt");
				}
				auto value = std::dynamic_pointer_cast<NumberObject>(args_[0]);
				if (!value) {
					throw std::runtime_error("Argument must be a number");
				}
				frame_.setReturnValue(std::sqrt(value->getValue()));
			}
			static void pow(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 2) {
					throw std::runtime_error("Not enough arguments for pow");
				}
				auto base = std::dynamic_pointer_cast<NumberObject>(args_[0]);
				auto exponent = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				if (!base || !exponent) {
					throw std::runtime_error("Arguments must be numbers");
				}
				frame_.setReturnValue(std::pow(base->getValue(), exponent->getValue()));
			}
			static void sin(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 1) {
					throw std::runtime_error("Not enough arguments for sin");
				}
				auto value = std::dynamic_pointer_cast<NumberObject>(args_[0]);
				if (!value) {
					throw std::runtime_error("Argument must be a number");
				}
				frame_.setReturnValue(std::sin(value->getValue()));
			}

			static void cos(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 1) {
					throw std::runtime_error("Not enough arguments for cos");
				}
				auto value = std::dynamic_pointer_cast<NumberObject>(args_[0]);
				if (!value) {
					throw std::runtime_error("Argument must be a number");
				}
				frame_.setReturnValue(std::cos(value->getValue()));
			}

			static void tan(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 1) {
					throw std::runtime_error("Not enough arguments for tan");
				}
				auto value = std::dynamic_pointer_cast<NumberObject>(args_[0]);
				if (!value) {
					throw std::runtime_error("Argument must be a number");
				}
				frame_.setReturnValue(std::tan(value->getValue()));
			}
	};
}  // namespace

namespace java {
	namespace lang {
		void Math(::sandvik::ClassLoader& classLoader) {
			ClassBuilder builder(classLoader, "java.lang", "java.lang.Math");
			builder.addMethod("abs", "(I)I", ACCESS_FLAGS::ACC_STATIC, Math::abs);
			builder.addMethod("max", "(II)I", ACCESS_FLAGS::ACC_STATIC, Math::max);
			builder.addMethod("min", "(II)I", ACCESS_FLAGS::ACC_STATIC, Math::min);
			builder.addMethod("sqrt", "(D)D", ACCESS_FLAGS::ACC_STATIC, Math::sqrt);
			builder.addMethod("sin", "(D)D", ACCESS_FLAGS::ACC_STATIC, Math::sin);
			builder.addMethod("cos", "(D)D", ACCESS_FLAGS::ACC_STATIC, Math::cos);
			builder.addMethod("tan", "(D)D", ACCESS_FLAGS::ACC_STATIC, Math::tan);
			builder.addMethod("pow", "(DD)D", ACCESS_FLAGS::ACC_STATIC, Math::pow);
			builder.finalize();
		}
	}  // namespace lang
}  // namespace java