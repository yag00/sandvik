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

#include "jni/jni.h"

#include "array.hpp"
#include "class.hpp"
#include "classbuilder.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "frame.hpp"
#include "interpreter.hpp"
#include "jni.hpp"
#include "jthread.hpp"
#include "method.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "system/sharedlibrary.hpp"
#include "vm.hpp"

using namespace sandvik;

Vm::Vm() : _classloader(std::make_unique<ClassLoader>()), _jnienv(std::make_unique<NativeInterface>(*this)) {
	logger.info("VM instance created.");

	ClassBuilder(*_classloader, "", "boolean").finalize();
	ClassBuilder(*_classloader, "", "byte").finalize();
	ClassBuilder(*_classloader, "", "char").finalize();
	ClassBuilder(*_classloader, "", "short").finalize();
	ClassBuilder(*_classloader, "", "int").finalize();
	ClassBuilder(*_classloader, "", "long").finalize();
	ClassBuilder(*_classloader, "", "float").finalize();
	ClassBuilder(*_classloader, "", "double").finalize();
	ClassBuilder(*_classloader, "", "void").finalize();

	// load self as a shared library to be able to resolve native methods
	loadLibrary("");
}

Vm::~Vm() {
	logger.debug("VM instance destroyed.");
}

void Vm::loadRt(const std::string& path) {
	_classloader->loadRt(path);
	if (!_isPrimitiveClassInitialized) {
		_isPrimitiveClassInitialized = true;
		/*
		| Wrapper Class         | Primitive Type | Descriptor | `TYPE` should point to… |
		| --------------------- | -------------- | ---------- | ----------------------- |
		| `java/lang/Boolean`   | `boolean`      | `Z`        | `boolean.class`         |
		| `java/lang/Byte`      | `byte`         | `B`        | `byte.class`            |
		| `java/lang/Character` | `char`         | `C`        | `char.class`            |
		| `java/lang/Short`     | `short`        | `S`        | `short.class`           |
		| `java/lang/Integer`   | `int`          | `I`        | `int.class`             |
		| `java/lang/Long`      | `long`         | `J`        | `long.class`            |
		| `java/lang/Float`     | `float`        | `F`        | `float.class`           |
		| `java/lang/Double`    | `double`       | `D`        | `double.class`          |
		| `java/lang/Void`      | `void`         | `V`        | `void.class`            |
		*/
		// Initialize primitive types and set TYPE field for each wrapper class
		const std::vector<std::pair<std::string, std::string>> primitives = {
		    {"java.lang.Boolean", "boolean"}, {"java.lang.Byte", "byte"},     {"java.lang.Character", "char"},
		    {"java.lang.Short", "short"},     {"java.lang.Integer", "int"},   {"java.lang.Long", "long"},
		    {"java.lang.Float", "float"},     {"java.lang.Double", "double"}, {"java.lang.Void", "void"}};
		for (const auto& [wrapper, primitive] : primitives) {
			try {
				auto& wrapperCls = _classloader->getOrLoad(wrapper);
				auto& primitiveCls = _classloader->getOrLoad(primitive);
				wrapperCls.getField("TYPE").setObjectValue(Object::makeConstClass(*_classloader, primitiveCls));
			} catch (const std::exception& e) {
				logger.error(e.what());
			}
		}
	}
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

ClassLoader& Vm::getClassLoader() const {
	return *_classloader;
}

void Vm::loadLibrary(const std::string& libName_) {
	// Load the shared library
	auto lib = std::make_unique<SharedLibrary>(libName_);
	lib->load();
	if (lib->isLoaded()) {
		if (!libName_.empty()) {
			logger.fdebug("Loaded shared library {}", lib->getFullPath());
		}
		// Call JNI_OnLoad if it exists
		void* JNI_onLoad = lib->getAddressOfSymbol("JNI_OnLoad");
		if (!JNI_onLoad) {
			logger.fdebug("JNI_OnLoad not found in {}", lib->getFullPath());
		} else {
			// jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
			auto jniOnLoad = reinterpret_cast<void (*)(JavaVM*, void*)>(JNI_onLoad);
			logger.fdebug("Executing native function JNI_OnLoad@{:#x} ", (uintptr_t)jniOnLoad);
			jniOnLoad(nullptr, nullptr);  // todo pass actual JavaVM
		}
		_sharedlibs.push_back(std::move(lib));
	} else {
		throw VmException("Failed to load shared library {}", libName_);
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

	JThread& mainThread = newThread("main");

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
			throw VmException("onCreate or main method not found in class {}", clazz_.getFullname());
		}
	}

	// Set the arguments for the main method
	const auto& stringClass = _classloader->getOrLoad("java.lang.String");
	auto args = Array::make(stringClass, (uint32_t)args_.size());
	for (size_t i = 0; i < args_.size(); ++i) {
		auto strObj = Object::make(*_classloader, args_[i]);
		std::static_pointer_cast<Array>(args)->setElement((uint32_t)i, strObj);
	}
	mainThread.currentFrame().setObjRegister(nbRegisters, args);
	try {
		mainThread.newFrame(clazz_.getMethod("<clinit>", "()V"));
	} catch (const std::exception& e) {
		logger.debug(e.what());
	}
	_isRunning.store(true);
	mainThread.run(true);
	_isRunning.store(false);
}

void Vm::stop() {
	logger.info("Stopping VM...");
	_isRunning.store(false);
}

JThread& Vm::newThread(const std::string& name_) {
	std::unique_lock<std::mutex> lock(_mutex);
	_threads.emplace_back(std::make_unique<JThread>(*this, *_classloader, name_));
	return *(_threads.back());
}

JThread& Vm::newThread(std::shared_ptr<Object> thread_) {
	std::unique_lock<std::mutex> lock(_mutex);
	_threads.emplace_back(std::make_unique<JThread>(*this, *_classloader, thread_));
	return *(_threads.back());
}

JThread& Vm::getThread(const std::string& name_) {
	std::unique_lock<std::mutex> lock(_mutex);
	for (const auto& thread : _threads) {
		if (thread->getName() == name_) {
			return *thread;
		}
	}
	throw VmException("Thread with name '{}' not found", name_);
}

JThread& Vm::currentThread() const {
	std::unique_lock<std::mutex> lock(_mutex);
	auto currentId = std::this_thread::get_id();
	for (const auto& thread : _threads) {
		if (thread->getId() == currentId) {
			return *thread;
		}
	}
	throw VmException("Current thread not found in VM");
}

void Vm::deleteThread(const std::string& name_) {
	std::unique_lock<std::mutex> lock(_mutex);
	for (auto it = _threads.begin(); it != _threads.end(); ++it) {
		if ((*it)->getName() == name_) {
			_threads.erase(it);
			return;
		}
	}
}

std::string Vm::getProperty(const std::string& name_) const {
	auto it = _properties.find(name_);
	if (it != _properties.end()) {
		return it->second;
	}
	throw VmException("Property '{}' not found", name_);
}

void Vm::setProperty(const std::string& name_, const std::string& value_) {
	_properties[name_] = value_;
}
