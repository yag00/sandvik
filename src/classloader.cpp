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

#include "classloader.hpp"

#include <algorithm>
#include <filesystem>
#include <sstream>

#include "array.hpp"
#include "class.hpp"
#include "classbuilder.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "frame.hpp"
#include "jthread.hpp"
#include "loader/apk.hpp"
#include "loader/dex.hpp"
#include "loader/rtld.hpp"
#include "method.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "system/os_constants.hpp"
#include "types.hpp"
#include "utils.hpp"
#include "vm.hpp"

using namespace sandvik;

ClassLoader::ClassLoader() = default;

ClassLoader::~ClassLoader() = default;

void ClassLoader::loadRtByDir(const std::string& rt_) {
	try {
		rtld::load(rt_, _dexs, &_jars);
		logger.fdebug("RT loaded: {}", rt_);
	} catch (const std::exception& e) {
		logger.ferror("Failed to load DEX: {}", e.what());
		return;
	}
}

void ClassLoader::loadRtByFile(const std::string& rt_) {
	try {
		rtld::loadJar(rt_, _dexs);
		_jars.push_back(rt_);
		logger.fdebug("RT loaded: {}", rt_);
	} catch (const std::exception& e) {
		logger.ferror("Failed to load DEX: {}", e.what());
		return;
	}
}

void ClassLoader::loadDex(const std::string& dex_) {
	try {
		auto dex = std::make_unique<Dex>(dex_);
		logger.fdebug("DEX loaded: {}", dex->getPath());
		_dexs.push_back(std::move(dex));
	} catch (const std::exception& e) {
		logger.ferror("Failed to load DEX: {}", e.what());
		return;
	}
}

void ClassLoader::loadApk(const std::string& apk_) {
	try {
		auto apk = std::make_unique<Apk>(apk_, _dexs);
		logger.fdebug("APK loaded: {}", apk->getPath());
		_apks.push_back(std::move(apk));
	} catch (const std::exception& e) {
		logger.ferror("Failed to load APK: {}", e.what());
		return;
	}
}

std::optional<std::vector<uint8_t>> ClassLoader::findResource(const std::string& name) {
	for (const auto& jarPath : _jars) {
		if (auto data = rtld::findResourceInJar(jarPath, name)) {
			return data;
		}
	}
	return std::nullopt;
}

void ClassLoader::addClassPath(const std::string& classpath_) {
	if (std::find(_classpath.begin(), _classpath.end(), classpath_) == _classpath.end()) {
		logger.fdebug("classpath add {}", classpath_);
		_classpath.push_back(classpath_);
	} else {
		logger.fdebug("classpath already exists: {}", classpath_);
	}
}

std::string ClassLoader::getClassPath() const {
	std::ostringstream oss;
	for (size_t i = 0; i < _classpath.size(); ++i) {
		oss << _classpath[i];
		if (i != _classpath.size() - 1) {
			oss << ":";
		}
	}
	return oss.str();
}

const std::vector<std::string>& ClassLoader::getClassPathEntries() const {
	return _classpath;
}

std::string ClassLoader::getMainActivity() const {
	for (const auto& apk : _apks) {
		if (!apk->getMainActivity().empty()) {
			return apk->getMainActivity();
		}
	}
	return "";
}

const Apk* ClassLoader::getMainApk() const {
	for (const auto& apk : _apks) {
		if (!apk->getMainActivity().empty()) {
			return apk.get();
		}
	}
	return nullptr;
}

bool ClassLoader::isApkLoaded(const std::string& path_) const {
	std::error_code ec;
	const auto canonical = std::filesystem::weakly_canonical(path_, ec);
	for (const auto& apk : _apks) {
		if (std::filesystem::weakly_canonical(apk->getPath(), ec) == canonical) {
			return true;
		}
	}
	return false;
}

void ClassLoader::addClass(std::unique_ptr<Class> class_) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	_classes[class_->getFullname()] = std::move(class_);
}

bool ClassLoader::isLoaded(const std::string& classname_) const {
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	auto normalized = classname_;
	if (normalized.size() > 2 && normalized.front() == 'L' && normalized.back() == ';') {
		normalized = normalized.substr(1, normalized.size() - 2);
	}
	std::replace(normalized.begin(), normalized.end(), '/', '.');
	return _classes.find(normalized) != _classes.end();
}

Class& ClassLoader::getOrLoad(const std::string& classname_) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	auto normalized = classname_;
	// Accept object descriptors (e.g. Ljava/lang/String;) in addition to binary names.
	if (normalized.size() > 2 && normalized.front() == 'L' && normalized.back() == ';') {
		normalized = normalized.substr(1, normalized.size() - 2);
	}
	auto dotclassname = normalized;
	std::replace(dotclassname.begin(), dotclassname.end(), '/', '.');
	// Check if the class is already loaded
	auto it = _classes.find(dotclassname);
	if (it != _classes.end()) {
		return *(it->second);
	}
	for (const auto& dex : _dexs) {
		try {
			auto classPtr = dex->findClass(*this, dotclassname);
			if (classPtr->isExternal()) {
				continue;
			}
			linkClass(*classPtr);
			_classes[dotclassname] = std::move(classPtr);
			return *(_classes[dotclassname]);
		} catch (std::exception& e) {
			// pass
		}
	}
	for (auto& classpath : _classpath) {
		try {
			auto slashclassname = normalized;
			std::replace(slashclassname.begin(), slashclassname.end(), '.', '/');
			std::string fullPath = classpath;
			if (fullPath.back() != '/') {
				fullPath += '/';
			}
			fullPath += slashclassname + ".dex";
			if (std::filesystem::exists(fullPath)) {
				auto dex = std::make_unique<Dex>(fullPath);
				_classes[dotclassname] = dex->findClass(*this, dotclassname);
				_dexs.push_back(std::move(dex));
				linkClass(*(_classes[dotclassname]));
				return *(_classes[dotclassname]);
			}
		} catch (std::exception&) {
			// pass
		}
	}
	// Handle native array types like [B, [I, etc., including multi-dimensional arrays
	// Create a synthetic array Class object
	if (dotclassname[0] == '[') {
		// Determine component descriptor
		std::string componentDesc = dotclassname.substr(1);
		// Load component class (make sure it exists)
		if (componentDesc[0] == 'L') {
			// Object array: [Ljava/lang/String;
			componentDesc = componentDesc.substr(1, componentDesc.size() - 2);
			getOrLoad(componentDesc);
		} else if (componentDesc[0] == '[') {
			// Multi-dimensional array
			getOrLoad(componentDesc);
		}

		auto builder = ClassBuilder(*this, "", dotclassname);
		builder.setSuperClass("java.lang.Object");
		builder.addInterface("java.lang.Cloneable");
		builder.addInterface("java.io.Serializable");
		builder.setArray(true);
		builder.setComponentType(componentDesc);
		builder.finalize();
		return *(_classes[dotclassname]);
	}
	// Handle primitive types
	if (dotclassname.size() == 1) {
		std::string type(1, dotclassname[0]);
		auto primitiveType = get_primitive_type(type);
		return getOrLoad(primitiveType);
	}

	// If the class is not found, throw an exception
	throw ClassNotFoundException(fmt::format("Class not found: {} ", dotclassname));
}

Method& ClassLoader::resolveMethod(uint32_t dex_, uint16_t idx_, std::string& classname_, std::string& method_, std::string& sig_) {
	if (dex_ >= _dexs.size()) {
		throw VmException("Invalid DEX index: {} (size: {})", dex_, _dexs.size());
	}
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	try {
		findMethod(dex_, idx_, classname_, method_, sig_);
		auto& entry = _methodRefCache[dex_][idx_];
		if (entry.resolved != nullptr) {
			return *entry.resolved;
		}
		auto& cls = getOrLoad(classname_);
		Method& method = cls.getMethod(method_, sig_);
		entry.resolved = &method;
		return method;
	} catch (std::exception& e) {
		throw VmException("Method {}.{}{} not found: {} ({})", classname_, method_, sig_, idx_, e.what());
	}
}
Method& ClassLoader::resolveMethod(uint32_t dex_, uint16_t idx_) {
	std::string classname;
	std::string method;
	std::string sig;
	return resolveMethod(dex_, idx_, classname, method, sig);
}
void ClassLoader::findMethod(uint32_t dex_, uint16_t idx_, std::string& classname_, std::string& method_, std::string& sig_) {
	if (dex_ >= _dexs.size()) {
		throw VmException("Invalid DEX index: {} (size: {})", dex_, _dexs.size());
	}
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	if (_methodRefCache.size() <= dex_) {
		_methodRefCache.resize(dex_ + 1);
	}
	auto& cache = _methodRefCache[dex_];
	auto it = cache.find(idx_);
	if (it == cache.end()) {
		_dexs[dex_]->resolveMethod(idx_, classname_, method_, sig_);
		cache.emplace(idx_, CachedMethodRef{classname_, method_, sig_, nullptr});
		return;
	}
	classname_ = it->second.classname;
	method_ = it->second.method;
	sig_ = it->second.sig;
}

Class& ClassLoader::resolveClass(uint32_t dex_, uint16_t idx_, std::string& classname_) {
	if (dex_ >= _dexs.size()) {
		throw VmException("Invalid DEX index: {} (size: {})", dex_, _dexs.size());
	}
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	try {
		if (_classRefCache.size() <= dex_) {
			_classRefCache.resize(dex_ + 1);
		}
		auto& cache = _classRefCache[dex_];
		auto it = cache.find(idx_);
		if (it == cache.end()) {
			_dexs[dex_]->resolveClass(idx_, classname_);
			it = cache.emplace(idx_, CachedClassRef{classname_, nullptr}).first;
		} else {
			classname_ = it->second.classname;
		}
		if (it->second.resolved != nullptr) {
			return *it->second.resolved;
		}
		auto& cls = getOrLoad(classname_);
		it->second.resolved = &cls;
		return cls;
	} catch (std::exception& e) {
		throw VmException("Class not found: {} ({})", idx_, e.what());
	}
}
Class& ClassLoader::resolveClass(uint32_t dex_, uint16_t idx_) {
	std::string classname;
	return resolveClass(dex_, idx_, classname);
}

Field& ClassLoader::resolveField(uint32_t dex_, uint16_t idx_) {
	std::string classname;
	std::string field;
	return resolveField(dex_, idx_, classname, field);
}

Field& ClassLoader::resolveField(uint32_t dex_, uint16_t idx_, std::string& classname_, std::string& field_) {
	if (dex_ >= _dexs.size()) {
		throw VmException("Invalid DEX index: {} (size: {})", dex_, _dexs.size());
	}
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	try {
		if (_fieldRefCache.size() <= dex_) {
			_fieldRefCache.resize(dex_ + 1);
		}
		auto& cache = _fieldRefCache[dex_];
		auto it = cache.find(idx_);
		if (it == cache.end()) {
			_dexs[dex_]->resolveField(idx_, classname_, field_);
			it = cache.emplace(idx_, CachedFieldRef{classname_, field_, nullptr}).first;
		} else {
			classname_ = it->second.classname;
			field_ = it->second.field;
		}
		if (it->second.resolved != nullptr) {
			return *it->second.resolved;
		}
		auto& cls = getOrLoad(classname_);
		Field& field = cls.getField(field_);
		it->second.resolved = &field;
		return field;
	} catch (std::exception& e) {
		throw VmException("Field not found: {} ({})", idx_, e.what());
	}
}

std::string ClassLoader::resolveType(uint32_t dex_, uint16_t idx_, TYPES& type_) {
	if (dex_ >= _dexs.size()) {
		throw VmException("Invalid DEX index: {} (size: {})", dex_, _dexs.size());
	}
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	try {
		if (_typeRefCache.size() <= dex_) {
			_typeRefCache.resize(dex_ + 1);
		}
		auto& cache = _typeRefCache[dex_];
		auto it = cache.find(idx_);
		if (it == cache.end()) {
			std::string name = _dexs[dex_]->resolveType(idx_, type_);
			it = cache.emplace(idx_, CachedTypeRef{name, type_}).first;
		}
		type_ = it->second.type;
		return it->second.name;
	} catch (const std::exception& e) {
		throw VmException("Type not found: {} ({})", idx_, e.what());
	}
}

std::string ClassLoader::resolveString(uint32_t dex_, uint16_t idx_) {
	if (dex_ >= _dexs.size()) {
		throw VmException("Invalid DEX index: {} (size: {})", dex_, _dexs.size());
	}
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	try {
		if (_stringRefCache.size() <= dex_) {
			_stringRefCache.resize(dex_ + 1);
		}
		auto& cache = _stringRefCache[dex_];
		auto it = cache.find(idx_);
		if (it == cache.end()) {
			it = cache.emplace(idx_, _dexs[dex_]->resolveString(idx_)).first;
		}
		return it->second;
	} catch (const std::exception& e) {
		throw VmException("String not found: {} ({})", idx_, e.what());
	}
}

std::vector<std::pair<std::string, uint32_t>> ClassLoader::resolveArray(uint32_t dex_, uint16_t idx_) {
	if (dex_ >= _dexs.size()) {
		throw VmException("Invalid DEX index: {} (size: {})", dex_, _dexs.size());
	}
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	try {
		if (_arrayRefCache.size() <= dex_) {
			_arrayRefCache.resize(dex_ + 1);
		}
		auto& cache = _arrayRefCache[dex_];
		auto it = cache.find(idx_);
		if (it == cache.end()) {
			it = cache.emplace(idx_, CachedArrayRef{_dexs[dex_]->resolveArray(idx_)}).first;
		}
		return it->second.descriptor;
	} catch (const std::exception& e) {
		throw VmException("Array not found: {} ({})", idx_, e.what());
	}
}

uint64_t ClassLoader::getDexIndex(const Dex& dex_) const {
	for (size_t i = 0; i < _dexs.size(); ++i) {
		if (_dexs[i].get() == &dex_) {
			return i;
		}
	}
	throw VmException("DEX not found in classloader");
}

void ClassLoader::setVm(Vm& vm_) {
	_vm = &vm_;
}

Vm& ClassLoader::getVm() const {
	if (!_vm) throw VmException("ClassLoader has no associated Vm");
	return *_vm;
}

void ClassLoader::visitReferences(const std::function<void(Object*)>& visitor_) const {
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	for (const auto& [name, classPtr] : _classes) {
		classPtr->visitReferences(visitor_);
	}
}

void ClassLoader::linkClass(Class& class_) {
	if (class_.getFullname() == "java.lang.Class") {
		class_.getMethod("getComponentType", "()Ljava/lang/Class;").makeNative();
		if (class_.hasMethod("getSuperclass", "()Ljava/lang/Class;")) {
			class_.getMethod("getSuperclass", "()Ljava/lang/Class;").makeNative();
		}
		if (class_.hasMethod("getEnumConstantsShared", "()[Ljava/lang/Object;")) {
			class_.getMethod("getEnumConstantsShared", "()[Ljava/lang/Object;").makeNative();
		}
		if (class_.hasMethod("getEnumConstantsShared", "()[Ljava/lang/Enum;")) {
			class_.getMethod("getEnumConstantsShared", "()[Ljava/lang/Enum;").makeNative();
		}
		return;
	}

	if (class_.getFullname() == "java.lang.Object") {
		class_.getMethod("getClass", "()Ljava/lang/Class;").makeNative();
		class_.getMethod("hashCode", "()I").makeNative();
		return;
	}

	if (class_.getFullname() == "java.lang.VMClassLoader") {
		class_.getMethod("getResource", "(Ljava/lang/String;)Ljava/net/URL;").makeNative();
		return;
	}

	if (class_.getFullname() == "android.system.OsConstants") {
		// placeholder() isn't actually native in this dex (ACC_NATIVE not set, body is "return 0"); hook it to feed real host values in field-declaration
		// order.
		class_.getMethod("placeholder", "()I").hook([](Frame& frame, std::vector<ObjectRef>& args) {
			(void)args;
			static thread_local size_t callIndex = 0;
			const auto& table = sandvik::osconst::orderedTable();
			if (callIndex >= table.size()) {
				callIndex = 0;
			}
			frame.setReturnValue(static_cast<int32_t>(table[callIndex++].value));
		});
		return;
	}

	if (class_.getFullname() == "java.lang.ref.ReferenceQueue") {
		auto& addMethod = class_.getMethod("add", "(Ljava/lang/ref/Reference;)V");
		addMethod.hook([](Frame& frame, std::vector<ObjectRef>& args) {
			// args: [0] = reference (static method, no 'this')
			if (args.size() != 1) {
				throw VmException("Invalid number of arguments for ReferenceQueue.add");
			}
			auto reference = args[0];
			if (reference->isNull()) {
				return;
			}
			// Sandvik has no real daemon thread draining the finalizer queue,
			// so we simulate it synchronously here: immediately invoke
			// finalize() on the reference's zombie/referent instead of truly
			// enqueueing for later async processing.
			auto zombie = reference->getField("zombie");
			if (zombie->isNull()) {
				return;
			}

			auto& cls = zombie->getClass();
			if (!cls.hasMethod("finalize", "()V")) {
				return;
			}
			auto& finalizeMethod = cls.getMethod("finalize", "()V");
			if (finalizeMethod.isNative() || !finalizeMethod.hasBytecode()) {
				return;
			}
			// Create a new thread to run the finalize method
			auto& classloader = cls.getClassLoader();
			auto& vm = classloader.getVm();
			auto& currentThread = vm.currentThread();
			auto& thread = currentThread.newChild(fmt::format("{}.finalize", cls.getFullname()));
			auto& newframe = thread.newFrame(finalizeMethod);
			newframe.setObjRegister(newframe.getMethod().getNbRegisters() - 1, zombie);
			thread.runInCurrentThread();
			currentThread.popChild();
		});
		return;
	}
}