#include "jthread.hpp"

#include <fmt/color.h>
#include <fmt/format.h>

#include "class.hpp"
#include "frame.hpp"
#include "interpreter.hpp"
#include "method.hpp"
#include "system/logger.hpp"

using namespace sandvik;

JThread::JThread(Vm& vm_, ClassLoader& classloader_, const std::string& name_)
    : _vm(vm_), _classloader(classloader_), _name(name_), _interpreter(std::make_unique<Interpreter>(*this)) {
}

ClassLoader& JThread::getClassLoader() const {
	return _classloader;
}

bool JThread::end() const {
	return _stack.size() == 0;
}

Frame& JThread::newFrame(Method& method_) {
	_stack.push_back(std::make_unique<Frame>(method_));
	return *(_stack.back().get());
}

void JThread::popFrame() {
	_stack.pop_back();
}

Frame& JThread::currentFrame() const {
	if (_stack.empty()) {
		throw std::runtime_error("No current frame");
	}
	return *(_stack.back().get());
}

void JThread::execute() {
	_interpreter->execute();
}

bool JThread::handleConstructor(const std::string& class_, const std::string& method_, const std::string& sig_, std::vector<std::shared_ptr<Object>> args_) {
	// logger.warning("TODO handleConstructor: " + class_ + " " + method_ + " " + sig_);
	return false;
}
bool JThread::handleInstanceMethod(Frame& frame_, const std::string& class_, const std::string& method_, const std::string& sig_,
                                   std::vector<std::shared_ptr<Object>> args_) {
	if (class_ == "java.lang.RuntimeException") {
		if (method_ == "getMessage" && sig_ == "()Ljava/lang/String;") {
			logger.debug(args_[0]->debug());
			auto clazz = std::dynamic_pointer_cast<ObjectClass>(args_[0]);
			if (clazz != nullptr) {
				logger.warning(fmt::format("java.lang.RuntimeException -> Not initialized"));
				frame_.setReturnObject(StringObject::make("Test"));
				return true;
			}
		}
	}
	if (class_ == "java.lang.IllegalArgumentException") {
		if (method_ == "getMessage" && sig_ == "()Ljava/lang/String;") {
			logger.debug(args_[0]->debug());
			auto clazz = std::dynamic_pointer_cast<ObjectClass>(args_[0]);
			if (clazz != nullptr) {
				logger.warning(fmt::format("java.lang.IllegalArgumentException -> Not initialized"));
				frame_.setReturnObject(StringObject::make("Invalid"));
				return true;
			}
		}
	}
	if (class_ == "java.lang.Class") {
		if (method_ == "getName" && sig_ == "()Ljava/lang/String;") {
			logger.debug(args_[0]->debug());
			auto clazz = std::dynamic_pointer_cast<ObjectClass>(args_[0]);
			if (clazz != nullptr) {
				logger.debug(fmt::format("java.lang.Class.getName -> {}", clazz->getClass().getFullname()));
				frame_.setReturnObject(StringObject::make(clazz->getClass().getFullname()));
				return true;
			}
		}
	}
	if (class_ == "java.lang.String") {
		if (method_ == "equals" && sig_ == "(Ljava/lang/Object;)Z") {
			auto str0 = std::dynamic_pointer_cast<StringObject>(args_[0]);
			auto str1 = std::dynamic_pointer_cast<StringObject>(args_[1]);
			if (str0 != nullptr && str1 != nullptr) {
				logger.debug(fmt::format("String.equals {} == {}", str0->str(), str1->str()));
				if (str0->str() == str1->str()) {
					frame_.setReturnValue(1);
				} else {
					frame_.setReturnValue(0);
				}
				return true;
			}
		}
		if (method_ == "length" && sig_ == "()I") {
			auto str0 = std::dynamic_pointer_cast<StringObject>(args_[0]);
			if (str0 != nullptr) {
				frame_.setReturnValue(str0->str().length());
				return true;
			}
		}
	}
	if (class_ == "java.io.PrintStream") {
		if (args_[0]->isInstanceOf("java.lang.System.out")) {
			if (method_ == "println" && sig_ == "(I)V") {
				auto i = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				if (i != nullptr) {
					fmt::print("{}\n", i->getValue());
					fflush(stdout);
					return true;
				}
			}
			if (method_ == "println" && sig_ == "(J)V") {
				auto i = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				if (i != nullptr) {
					fmt::print("{}\n", i->getLongValue());
					fflush(stdout);
					return true;
				}
			}
			if (method_ == "println" && sig_ == "(Ljava/lang/String;)V") {
				auto str = std::dynamic_pointer_cast<StringObject>(args_[1]);
				if (str != nullptr) {
					fmt::print("{}\n", str->str());
					fflush(stdout);
					return true;
				}
			}
			if (method_ == "print" && sig_ == "(Ljava/lang/String;)V") {
				auto str = std::dynamic_pointer_cast<StringObject>(args_[1]);
				if (str != nullptr) {
					fmt::print("{}", str->str());
					fflush(stdout);
					return true;
				}
			}
		}
	}
	if (class_ == "java.lang.StringBuilder") {
		if (args_[0]->isInstanceOf("java.lang.StringBuilder")) {
			std::string* data = (std::string*)args_[0]->getObjectData();
			if (data == nullptr) {
				args_[0]->setObjectData((uintptr_t*)new std::string());
				data = (std::string*)args_[0]->getObjectData();
			}
			if (method_ == "append" && sig_ == "(Z)Ljava/lang/StringBuilder;") {
				auto i = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				logger.debug(fmt::format("StringBuilder append: {}", (bool)i->getValue()));
				data->append(std::to_string((bool)i->getValue()));
				frame_.setReturnObject(args_[0]);
				return true;
			}
			if (method_ == "append" && sig_ == "(C)Ljava/lang/StringBuilder;") {
				auto i = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				logger.debug(fmt::format("StringBuilder append: {}", (char)i->getValue()));
				data->append(std::to_string((char)i->getValue()));
				frame_.setReturnObject(args_[0]);
				return true;
			}
			if (method_ == "append" && sig_ == "(I)Ljava/lang/StringBuilder;") {
				auto i = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				logger.debug(fmt::format("StringBuilder append: {}", i->getValue()));
				data->append(std::to_string(i->getValue()));
				frame_.setReturnObject(args_[0]);
				return true;
			}
			if (method_ == "append" && sig_ == "(J)Ljava/lang/StringBuilder;") {
				auto lsb = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				auto msb = std::dynamic_pointer_cast<NumberObject>(args_[2]);
				uint64_t value = ((uint64_t)msb->getValue() << 32) | lsb->getValue();
				logger.debug(fmt::format("StringBuilder append: {}", value));
				data->append(std::to_string(value));
				frame_.setReturnObject(args_[0]);
				return true;
			}
			if (method_ == "append" && sig_ == "(F)Ljava/lang/StringBuilder;") {
				auto i = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				logger.debug(fmt::format("StringBuilder append: {}", i->getFloatValue()));
				data->append(std::to_string(i->getFloatValue()));
				frame_.setReturnObject(args_[0]);
				return true;
			}
			if (method_ == "append" && sig_ == "(D)Ljava/lang/StringBuilder;") {
				auto lsb = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				auto msb = std::dynamic_pointer_cast<NumberObject>(args_[2]);
				uint64_t value = ((uint64_t)msb->getValue() << 32) | lsb->getValue();
				double dvalue = *reinterpret_cast<double*>(&value);
				logger.debug(fmt::format("StringBuilder append: {}", dvalue));
				data->append(std::to_string(dvalue));
				frame_.setReturnObject(args_[0]);
				return true;
			}
			if (method_ == "append" && sig_ == "(Ljava/lang/String;)Ljava/lang/StringBuilder;") {
				auto str = std::dynamic_pointer_cast<StringObject>(args_[1]);
				logger.debug(fmt::format("StringBuilder append: {}", str->str()));
				data->append(str->str());
				frame_.setReturnObject(args_[0]);
				return true;
			}
			if (method_ == "toString" && sig_ == "()Ljava/lang/String;") {
				logger.debug(fmt::format("StringBuilder toString: {}", *data));
				frame_.setReturnObject(Object::make(*data));
				return true;
			}
		}
		logger.error(fmt::format("{}", args_[0]->debug()));
	}
	logger.warning("TODO handleInstanceMethod: " + class_ + " " + method_ + " " + sig_);
	return false;
}

bool JThread::handleClassFieldGetter(const std::string& class_, const std::string& field_) {
	if (class_ == "java.lang.System" && field_ == "out") {
		return true;
	}
	return false;
}
