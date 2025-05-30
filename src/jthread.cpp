#include "jthread.hpp"

#include <fmt/color.h>
#include <fmt/format.h>

#include "class.hpp"
#include "frame.hpp"
#include "interpreter.hpp"
#include "method.hpp"
#include "system/logger.hpp"
#include "vm.hpp"

using namespace sandvik;

JThread::JThread(Vm& vm_, ClassLoader& classloader_, const std::string& name_)
    : _vm(vm_), _classloader(classloader_), _name(name_), _interpreter(std::make_unique<Interpreter>(*this)) {
}

Vm& JThread::vm() const {
	return _vm;
}

ClassLoader& JThread::getClassLoader() const {
	return _classloader;
}

bool JThread::end() const {
	return _stack.size() == 0;
}

uint64_t JThread::stackDepth() const {
	return _stack.size();
}

Frame& JThread::newFrame(Method& method_) {
	if (method_.getName() == "<clinit>") {
		auto& clazz = method_.getClass();
		if (!clazz.isStaticInitialized()) {
			// method clinit has been pushed on the stack and will be executed, mark the class as initialized
			clazz.setStaticInitialized();
		} else {
			logger.warning(fmt::format("Class {} already initialized", clazz.getFullname()));
		}
	}
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
