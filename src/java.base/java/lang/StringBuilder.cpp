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
	class StringBuilder {
		public:
			static std::shared_ptr<std::string> __append_check_and_get_string(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 2) {
					throw std::runtime_error("Not enough arguments");
				}
				auto clazz = std::dynamic_pointer_cast<ObjectClass>(args_[0]);
				if (!clazz || !clazz->isInstanceOf("java.lang.StringBuilder")) {
					throw std::runtime_error("First argument is not an instance of java.lang.StringBuilder");
				}
				auto message = clazz->getObjectData<std::string>();
				if (!message) throw std::runtime_error("StringBuilder object data is not initialized");
				return message;
			}
			static void init(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 1) {
					throw std::runtime_error("Not enough arguments");
				}
				auto clazz = std::dynamic_pointer_cast<ObjectClass>(args_[0]);
				if (!clazz || !clazz->isInstanceOf("java.lang.StringBuilder")) {
					throw std::runtime_error("First argument is not an instance of java.lang.StringBuilder");
				}
				clazz->setObjectData(std::make_shared<std::string>());
			}
			static void append_Z(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				auto s = __append_check_and_get_string(frame_, args_);
				auto i = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				if (i == nullptr) throw std::runtime_error("Second argument is not a boolean (Z) value");
				s->append(std::to_string((bool)i->getValue()));
				frame_.setReturnObject(args_[0]);
			}
			static void append_C(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				auto s = __append_check_and_get_string(frame_, args_);
				auto i = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				if (i == nullptr) throw std::runtime_error("Second argument is not a char (C) value");
				s->append(std::to_string((char)i->getValue()));
				frame_.setReturnObject(args_[0]);
			}
			static void append_I(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				auto s = __append_check_and_get_string(frame_, args_);
				auto i = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				if (i == nullptr) throw std::runtime_error("Second argument is not an integer (I) value");
				s->append(std::to_string(i->getValue()));
				frame_.setReturnObject(args_[0]);
			}
			static void append_J(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				auto s = __append_check_and_get_string(frame_, args_);
				auto lsb = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				auto msb = std::dynamic_pointer_cast<NumberObject>(args_[2]);
				if (lsb == nullptr || msb == nullptr) {
					throw std::runtime_error("Second or third argument is not a valid integer (J) value");
				}
				int64_t value = (static_cast<uint64_t>((uint32_t)msb->getValue()) << 32) | static_cast<uint64_t>((uint32_t)lsb->getValue());
				s->append(std::to_string(value));
				frame_.setReturnObject(args_[0]);
			}
			static void append_F(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				auto s = __append_check_and_get_string(frame_, args_);
				auto i = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				if (i == nullptr) throw std::runtime_error("Second argument is not a float (F) value");
				s->append(std::to_string(i->getFloatValue()));
				frame_.setReturnObject(args_[0]);
			}
			static void append_D(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				auto s = __append_check_and_get_string(frame_, args_);
				auto lsb = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				auto msb = std::dynamic_pointer_cast<NumberObject>(args_[2]);
				if (lsb == nullptr || msb == nullptr) {
					throw std::runtime_error("Second or third argument is not a valid double (D) value");
				}
				uint64_t value = (static_cast<uint64_t>((uint32_t)msb->getValue()) << 32) | static_cast<uint64_t>((uint32_t)lsb->getValue());
				double dvalue = *reinterpret_cast<double*>(&value);
				s->append(std::to_string(dvalue));
				frame_.setReturnObject(args_[0]);
			}
			static void append_String(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				auto s = __append_check_and_get_string(frame_, args_);
				auto str = std::dynamic_pointer_cast<StringObject>(args_[1]);
				if (str == nullptr) {
					throw std::runtime_error("Second argument is not a valid String object");
				}
				s->append(str->str());
				frame_.setReturnObject(args_[0]);
			}
			static void toString(::sandvik::ClassLoader& loader_, Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 1) {
					throw std::runtime_error("Not enough arguments");
				}
				auto clazz = std::dynamic_pointer_cast<ObjectClass>(args_[0]);
				if (!clazz || !clazz->isInstanceOf("java.lang.StringBuilder")) {
					throw std::runtime_error("First argument is not an instance of java.lang.StringBuilder");
				}
				auto str = clazz->getObjectData<std::string>();
				if (!str) {
					throw std::runtime_error("StringBuilder object data is not initialized");
				}
				frame_.setReturnObject(Object::make(loader_, *str));
			}

		public:
	};
}  // namespace

namespace java {
	namespace lang {
		void StringBuilder(::sandvik::ClassLoader& classLoader) {
			ClassBuilder builder(classLoader, "java.lang", "java.lang.StringBuilder");
			builder.addVirtualMethod("<init>", "()V", 0, StringBuilder::init);
			builder.addVirtualMethod("append", "(Z)Ljava/lang/StringBuilder;", 0, StringBuilder::append_Z);
			builder.addVirtualMethod("append", "(C)Ljava/lang/StringBuilder;", 0, StringBuilder::append_C);
			builder.addVirtualMethod("append", "(I)Ljava/lang/StringBuilder;", 0, StringBuilder::append_I);
			builder.addVirtualMethod("append", "(J)Ljava/lang/StringBuilder;", 0, StringBuilder::append_J);
			builder.addVirtualMethod("append", "(F)Ljava/lang/StringBuilder;", 0, StringBuilder::append_F);
			builder.addVirtualMethod("append", "(D)Ljava/lang/StringBuilder;", 0, StringBuilder::append_D);
			builder.addVirtualMethod("append", "(Ljava/lang/String;)Ljava/lang/StringBuilder;", 0, StringBuilder::append_String);
			builder.addVirtualMethod(
			    "toString", "()Ljava/lang/String;", 0,
			    [&classLoader](Frame& frame_, std::vector<std::shared_ptr<::sandvik::Object>>& args_) { StringBuilder::toString(classLoader, frame_, args_); });
			builder.finalize();
		}
	}  // namespace lang
}  // namespace java