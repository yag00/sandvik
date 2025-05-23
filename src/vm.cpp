#include "vm.hpp"

#include <fmt/format.h>

#include <iostream>

#include "jni/jni.h"

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "frame.hpp"
#include "interpreter.hpp"
#include "jni.hpp"
#include "jthread.hpp"
#include "method.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "system/sharedlibrary.hpp"

using namespace sandvik;

Vm::Vm() : _classloader(std::make_unique<ClassLoader>()), _jnienv(std::make_unique<NativeInterface>()) {
	logger.info("VM instance created.");
}

void Vm::loadDex(const std::string& path) {
	_classloader->loadDex(path);
}

void Vm::loadApk(const std::string& path) {
	_classloader->loadApk(path);
}

void Vm::addClassPath(const std::string& classpath_) {
	_classloader->addClassPath(classpath_);
}

std::string Vm::getClassPath() const {
	return _classloader->getClassPath();
}

void Vm::loadLibrary(const std::string& libName_) {
	// Load the shared library
	auto lib = std::make_unique<SharedLibrary>(libName_);
	lib->load();
	if (lib->isLoaded()) {
		logger.debug(fmt::format("Loaded shared library {}", lib->getFullPath()));
		// Call JNI_OnLoad if it exists
		void* JNI_onLoad = lib->getAddressOfSymbol("JNI_OnLoad");
		if (!JNI_onLoad) {
			logger.debug(fmt::format("JNI_OnLoad not found in {}", lib->getFullPath()));
		} else {
			// jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
			auto jniOnLoad = reinterpret_cast<void (*)(JavaVM*, void*)>(JNI_onLoad);
			logger.debug(fmt::format("Executing native function JNI_OnLoad@{:#x} ", (uintptr_t)jniOnLoad));
			jniOnLoad(nullptr, nullptr);  // todo pass actual JavaVM
		}
		_sharedlibs.push_back(std::move(lib));
	} else {
		throw std::runtime_error(fmt::format("Failed to load shared library {}", libName_));
	}
}

void* Vm::findNativeSymbol(const std::string& symbolName_) {
	for (const auto& lib : _sharedlibs) {
		void* symbol = lib->getAddressOfSymbol(symbolName_);
		if (symbol) {
			return symbol;
		}
	}
	return nullptr;
}

NativeInterface* Vm::getJNIEnv() const {
	return _jnienv.get();
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