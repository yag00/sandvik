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

#include "class.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "loader/apk.hpp"
#include "loader/dex.hpp"
#include "loader/rtld.hpp"
#include "method.hpp"
#include "system/logger.hpp"
#include "types.hpp"

using namespace sandvik;

ClassLoader::ClassLoader() {
}

ClassLoader::~ClassLoader() {
}

void ClassLoader::loadRt(const std::string& rt_) {
	try {
		rtld::load(rt_, _dexs);
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

std::string ClassLoader::getMainActivity() const {
	for (const auto& apk : _apks) {
		if (!apk->getMainActivity().empty()) {
			return apk->getMainActivity();
		}
	}
	return "";
}

Class& ClassLoader::getMainActivityClass() {
	for (const auto& apk : _apks) {
		auto className = apk->getMainActivity();
		if (!className.empty()) {
			return getOrLoad(className);
		}
	}
	throw VmException("Main activity class not found");
}

void ClassLoader::addClass(std::unique_ptr<Class> class_) {
	_classes[class_->getFullname()] = std::move(class_);
}

Class& ClassLoader::getOrLoad(const std::string& classname_) {
	auto dotclassname = classname_;
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
			_classes[dotclassname] = std::move(classPtr);
			return *(_classes[dotclassname]);
		} catch (std::exception& e) {
			// pass
		}
	}
	for (auto& classpath : _classpath) {
		try {
			auto slashclassname = classname_;
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
				logger.fok("class {} loaded", dotclassname);
				return *(_classes[dotclassname]);
			}
		} catch (std::exception&) {
			// pass
		}
	}
	// If the class is not found, throw an exception
	throw VmException("ClassNotFoundError: {}", dotclassname);
}

Method& ClassLoader::resolveMethod(uint32_t dex_, uint16_t idx_, std::string& classname_, std::string& method_, std::string& sig_) {
	if (dex_ >= _dexs.size()) {
		throw VmException("Invalid DEX index: {} (size: {})", dex_, _dexs.size());
	}
	try {
		_dexs[dex_]->resolveMethod(idx_, classname_, method_, sig_);
		auto& cls = getOrLoad(classname_);
		return cls.getMethod(method_, sig_);
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
	_dexs[dex_]->resolveMethod(idx_, classname_, method_, sig_);
}

Class& ClassLoader::resolveClass(uint32_t dex_, uint16_t idx_, std::string& classname_) {
	if (dex_ >= _dexs.size()) {
		throw VmException("Invalid DEX index: {} (size: {})", dex_, _dexs.size());
	}
	try {
		_dexs[dex_]->resolveClass(idx_, classname_);
		return getOrLoad(classname_);
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
	try {
		_dexs[dex_]->resolveField(idx_, classname_, field_);
		auto& cls = getOrLoad(classname_);
		return cls.getField(field_);
	} catch (std::exception& e) {
		throw VmException("Field not found: {} ({})", idx_, e.what());
	}
}

std::string ClassLoader::resolveType(uint32_t dex_, uint16_t idx_, TYPES& type_) {
	if (dex_ >= _dexs.size()) {
		throw VmException("Invalid DEX index: {} (size: {})", dex_, _dexs.size());
	}
	try {
		return _dexs[dex_]->resolveType(idx_, type_);
	} catch (const std::exception& e) {
		throw VmException("Type not found: {} ({})", idx_, e.what());
	}
}

std::string ClassLoader::resolveString(uint32_t dex_, uint16_t idx_) {
	if (dex_ >= _dexs.size()) {
		throw VmException("Invalid DEX index: {} (size: {})", dex_, _dexs.size());
	}
	try {
		return _dexs[dex_]->resolveString(idx_);
	} catch (const std::exception& e) {
		throw VmException("String not found: {} ({})", idx_, e.what());
	}
}

std::vector<std::pair<std::string, uint32_t>> ClassLoader::resolveArray(uint32_t dex_, uint16_t idx_) {
	if (dex_ >= _dexs.size()) {
		throw VmException("Invalid DEX index: {} (size: {})", dex_, _dexs.size());
	}
	try {
		return _dexs[dex_]->resolveArray(idx_);
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

void ClassLoader::visitReferences(const std::function<void(Object*)>& visitor_) const {
	for (const auto& [name, classPtr] : _classes) {
		classPtr->visitReferences(visitor_);
	}
}