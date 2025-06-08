/*
 * This file is part of Sandvik project.
 * Copyright (C) 2025 Christophe Duvernois
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "vm.hpp"

#include <fmt/format.h>

#include <iostream>

#include "jni/jni.h"

#include "array.hpp"
#include "class.hpp"
#include "classbuilder.hpp"
#include "classloader.hpp"
#include "field.hpp"
#include "frame.hpp"
#include "interpreter.hpp"
#include "java.base/classes.hpp"
#include "jni.hpp"
#include "jthread.hpp"
#include "method.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "system/sharedlibrary.hpp"

using namespace sandvik;

Vm::Vm() : _classloader(std::make_unique<ClassLoader>()), _jnienv(std::make_unique<NativeInterface>()) {
	logger.info("VM instance created.");
	java::io::PrintStream(*_classloader);                 // Load java.io.PrintStream class
	java::lang::Class(*_classloader);                     // Load java.lang.Class class
	java::lang::Double(*_classloader);                    // Load java.lang.Class class
	java::lang::IllegalArgumentException(*_classloader);  // Load java.lang.Class class
	java::lang::Integer(*_classloader);                   // Load java.lang.Class class
	java::lang::Object(*_classloader);                    // Load java.lang.Object class
	java::lang::RuntimeException(*_classloader);          // Load java.lang.Object class
	java::lang::System(*this, *_classloader);             // Load java.lang.System class
	java::lang::String(*_classloader);                    // Load java.lang.String class
	java::lang::StringBuilder(*_classloader);             // Load java.lang.StringBuilder class
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
	run(clazz, {});
}
void Vm::run(const std::string& mainClass_, const std::vector<std::string>& args_) {
	auto& clazz = _classloader->getOrLoad(mainClass_);
	run(clazz, args_);
}

void Vm::run(Class& clazz_, const std::vector<std::string>& args_) {
	logger.info("Running class: " + clazz_.getFullname());
	JThread mainThread(*this, *_classloader, "main");

	uint32_t nbRegisters = 0;
	if (clazz_.hasMethod("onCreate", "(Landroid/os/Bundle;)V")) {
		// Create a new frame for the main method
		auto& method = clazz_.getMethod("onCreate", "(Landroid/os/Bundle;)V");
		mainThread.newFrame(method);
		nbRegisters = method.getNbRegisters() - 1;
	} else {
		if (clazz_.hasMethod("main", "([Ljava/lang/String;)V")) {
			// Create a new frame for the main method
			auto& method = clazz_.getMethod("main", "([Ljava/lang/String;)V");
			mainThread.newFrame(method);
			nbRegisters = method.getNbRegisters() - 1;
		} else {
			throw std::runtime_error(fmt::format("onCreate or main method not found in class {}", clazz_.getFullname()));
		}
	}

	// Set the arguments for the main method
	auto args = Array::make("String", args_.size());
	for (size_t i = 0; i < args_.size(); ++i) {
		auto strObj = StringObject::make(*_classloader, args_[i]);
		std::static_pointer_cast<Array>(args)->setArrayElement(i, strObj);
	}
	mainThread.currentFrame().setObjRegister(nbRegisters, args);
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