#include "vm.hpp"

#include <iostream>

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "frame.hpp"
#include "interpreter.hpp"
#include "jthread.hpp"
#include "method.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

Vm::Vm() : _classloader(std::make_unique<ClassLoader>()) {
	// Constructor logic if needed
	logger.info("VM instance created.");
}

void Vm::loadDex(const std::string& path) {
	_classloader->loadDex(path);
}

void Vm::loadApk(const std::string& path) {
	_classloader->loadApk(path);
}

void Vm::run() {
	auto& clazz = _classloader->getMainActivityClass();
	clazz.debug();
	run(clazz, {});
}
void Vm::run(const std::string& mainClass_, const std::vector<std::string>& args_) {
	auto& clazz = _classloader->getOrLoad(mainClass_);
	clazz.debug();
	run(clazz, args_);
}

void Vm::run(Class& clazz_, const std::vector<std::string>& args_) {
	logger.info("Running class: " + clazz_.getFullname());
	JThread mainThread(*this, *_classloader, "main");
	// Create a new frame for the main method
	auto& method = clazz_.getMethod("main", "([Ljava/lang/String;)V");
	mainThread.newFrame(method);
	// Set the arguments for the main method
	auto args = Array::make("String", args_.size());
	for (size_t i = 0; i < args_.size(); ++i) {
		auto strObj = StringObject::make(args_[i]);
		std::static_pointer_cast<Array>(args)->setArrayElement(i, strObj);
	}
	mainThread.currentFrame().setObjRegister(method.getNbRegisters() - 1, args);
	try {
		mainThread.newFrame(clazz_.getMethod("<clinit>", "()V"));
		clazz_.setStaticInitialized();
	} catch (const std::exception& e) {
		logger.debug(e.what());
	}
	while (!mainThread.end()) {
		mainThread.execute();
	}
}

void Vm::stop() {
	logger.info("Stopping VM...");
	// Add cleanup logic if necessary
}