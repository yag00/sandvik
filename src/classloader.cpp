#include "classloader.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <sstream>

#include "class.hpp"
#include "field.hpp"
#include "loader/apk.hpp"
#include "loader/dex.hpp"
#include "method.hpp"
#include "system/logger.hpp"

using namespace sandvik;

ClassLoader::ClassLoader() {
	// Constructor implementation
}

ClassLoader::~ClassLoader() {
	// Destructor implementation
}

void ClassLoader::loadDex(const std::string& dex_) {
	try {
		// Load the DEX file
		auto dex = std::make_unique<Dex>(_dexs.size(), dex_);  // Assuming Apk can handle DEX files as well
		logger.debug(fmt::format("DEX loaded: {}", dex->getPath()));
		_dexs.push_back(std::move(dex));
	} catch (const std::exception& e) {
		logger.error(fmt::format("Failed to load DEX: {}", e.what()));
		return;
	}
}

void ClassLoader::loadApk(const std::string& apk_) {
	try {
		auto apk = std::make_unique<Apk>(apk_);
		logger.debug(fmt::format("APK loaded: {}", apk->getPath()));
		_apks.push_back(std::move(apk));
	} catch (const std::exception& e) {
		logger.error(fmt::format("Failed to load APK: {}", e.what()));
		return;
	}
}

void ClassLoader::addClassPath(const std::string& classpath_) {
	if (std::find(_classpath.begin(), _classpath.end(), classpath_) == _classpath.end()) {
		logger.debug(fmt::format("classpath add {}", classpath_));
		_classpath.push_back(classpath_);
	} else {
		logger.debug(fmt::format("classpath already exists: {}", classpath_));
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
			auto it = _classes.find(className);
			if (it != _classes.end()) {
				return *(it->second);
			} else {
				auto classPtr = apk->findClass(className);
				_classes[className] = std::move(classPtr);
				return *(_classes[className]);
			}
		}
	}
	throw std::runtime_error("Main activity class not found");
}

Class& ClassLoader::getOrLoad(const std::string& classname_) {
	// Check if the class is already loaded
	auto it = _classes.find(classname_);
	if (it != _classes.end()) {
		return *(it->second);
	}
	for (const auto& apk : _apks) {
		try {
			auto classPtr = apk->findClass(classname_);
			_classes[classname_] = std::move(classPtr);
			return *(_classes[classname_]);
		} catch (std::exception& e) {
			// pass
		}
	}
	for (const auto& dex : _dexs) {
		try {
			auto classPtr = dex->findClass(classname_);
			_classes[classname_] = std::move(classPtr);
			return *(_classes[classname_]);
		} catch (std::exception& e) {
			// pass
		}
	}
	// If the class is not found, throw an exception
	throw std::runtime_error(fmt::format("ClassNotFoundError: {}", classname_));
}

Method& ClassLoader::resolveMethod(uint16_t idx, std::string& classname_, std::string& methodname_, std::string& sig_) {
	for (const auto& dex : _dexs) {
		try {
			dex->resolveMethod(idx, classname_, methodname_, sig_);
			auto& cls = getOrLoad(classname_);
			return cls.getMethod(methodname_, sig_);
		} catch (std::exception& e) {
			logger.error(fmt::format("Failed to resolve method {}: {}", idx, e.what()));
		}
	}
	throw std::runtime_error(fmt::format("Method not found: {}", idx));
}
Method& ClassLoader::resolveMethod(uint16_t idx) {
	std::string classname;
	std::string method;
	std::string sig;
	return resolveMethod(idx, classname, method, sig);
}

Class& ClassLoader::resolveClass(uint16_t idx, std::string& classname_) {
	for (const auto& dex : _dexs) {
		try {
			dex->resolveClass(idx, classname_);
			return getOrLoad(classname_);
		} catch (std::exception& e) {
			logger.error(fmt::format("Failed to resolve class {}: {}", idx, e.what()));
		}
	}
	throw std::runtime_error(fmt::format("Class not found: {}", idx));
}
Class& ClassLoader::resolveClass(uint16_t idx) {
	std::string classname;
	return resolveClass(idx, classname);
}

Field& ClassLoader::resolveField(uint16_t idx) {
	for (const auto& dex : _dexs) {
		try {
			std::string classname;
			std::string field;
			dex->resolveField(idx, classname, field);
			auto& cls = getOrLoad(classname);
			return cls.getField(field);
		} catch (std::exception& e) {
			logger.error(fmt::format("Failed to resolve field {}: {}", idx, e.what()));
		}
	}
	throw std::runtime_error(fmt::format("Field not found: {}", idx));
}

std::string ClassLoader::resolveString(uint16_t idx) {
	for (const auto& dex : _dexs) {
		try {
			return dex->resolveString(idx);
		} catch (const std::exception& e) {
			logger.error(fmt::format("Failed to resolve string {}: {}", idx, e.what()));
		}
	}
	throw std::runtime_error(fmt::format("String not found: {}", idx));
}

std::vector<std::pair<std::string, uint32_t>> ClassLoader::resolveArray(uint16_t idx) {
	for (const auto& dex : _dexs) {
		try {
			return dex->resolveArray(idx);
		} catch (const std::exception& e) {
			logger.error(fmt::format("Failed to resolve array {}: {}", idx, e.what()));
		}
	}
	throw std::runtime_error(fmt::format("Array not found: {}", idx));
}