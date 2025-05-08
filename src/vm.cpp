#include "vm.hpp"

#include <iostream>

#include "class.hpp"
#include "classloader.hpp"
#include "frame.hpp"
#include "interpreter.hpp"
#include "jthread.hpp"
#include "method.hpp"
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
	mainThread.newFrame(clazz_.getMethod("main", "([Ljava/lang/String;)V"));
	mainThread.newFrame(clazz_.getMethod("<clinit>", "()V"));
	while (!mainThread.end()) {
		mainThread.execute();
	}
}

void Vm::stop() {
	logger.info("Stopping VM...");
	// Add cleanup logic if necessary
}