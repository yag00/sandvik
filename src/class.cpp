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

#include "class.hpp"

#include <fmt/format.h>

#include <LIEF/DEX/Class.hpp>
#include <LIEF/DEX/Field.hpp>
#include <LIEF/DEX/Method.hpp>
#include <LIEF/DEX/Prototype.hpp>
#include <LIEF/DEX/Type.hpp>
#include <sstream>

#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "method.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "utils.hpp"

using namespace sandvik;

Class::Class(ClassLoader& classloader_, const std::string& packagename_, const std::string& fullname_)
    : _classloader(classloader_),
      _isStaticInitialized(false),
      _packagename(packagename_),
      _fullname(fullname_),
      _dexIdx(-1),
      _isInterface(false),
      _isAbstract(false),
      _hasSuperClass(false),
      _superClassname("") {
	// Extract class name from fullname
	auto pos = fullname_.find_last_of('.');
	if (pos != std::string::npos) {
		_name = fullname_.substr(pos + 1);
	} else {
		_name = fullname_;
	}
}

Class::Class(ClassLoader& classloader_, const uint32_t dexIdx_, const LIEF::DEX::Class& class_)
    : _classloader(classloader_),
      _isStaticInitialized(false),
      _packagename(class_.package_name()),
      _fullname(class_.pretty_name()),
      _name(class_.name()),
      _dexIdx(dexIdx_),
      _isInterface(class_.has(LIEF::DEX::ACC_INTERFACE)),
      _isAbstract(class_.has(LIEF::DEX::ACC_ABSTRACT)),
      _hasSuperClass(class_.has_parent()) {
	// Initialize methods
	for (const auto& method : class_.methods()) {
		const auto& name = method.name();
		auto signature = get_method_descriptor(method);
		_methods[name + signature] = std::make_unique<Method>(*this, method);
	}
	// Initialize fields
	for (const auto& field : class_.fields()) {
		_fields[field.name()] = std::make_unique<Field>(*this, field);
	}
	// Initialize parent classname if it exists
	_superClassname = class_.has_parent() ? class_.parent()->pretty_name() : "";
}

bool Class::isStaticInitialized() {
	if (_isStaticInitialized) {
		return true;
	}
	// Check if the class has a static initializer
	for (const auto& [name, method] : _methods) {
		if (method->isStatic() && method->getName() == "<clinit>") {
			return _isStaticInitialized;
		}
	}
	// no static initializer found, mark as initialized
	_isStaticInitialized = true;
	return _isStaticInitialized;
}

void Class::setStaticInitialized() {
	_isStaticInitialized = true;
}

uint32_t Class::getDexIdx() const {
	return _dexIdx;
}

std::string Class::getName() const {
	return _name;
}

std::string Class::getFullname() const {
	return _fullname;
}

bool Class::isInstanceOf(const std::string& classname_) const {
	if (getFullname() == classname_) {
		return true;
	}
	return false;
}
bool Class::isInstanceOf(const Class& class_) const {
	if (getFullname() == class_.getFullname()) {
		return true;
	}
	return false;
}
bool Class::isInstanceOf(const std::shared_ptr<Object>& class_) const {
	if (class_->isNull()) {
		return false;
	}
	if (auto clazz = class_; clazz->isClass()) {
		// check class and super classes
		while (true) {
			logger.fdebug("{} is instance of {}", clazz->getClass().getFullname(), getFullname());
			if (clazz->getClass().getFullname() == getFullname()) {
				return true;
			}
			if (!clazz->getClass().hasSuperClass()) break;
			clazz = Object::make(clazz->getClass().getSuperClass());
		}
		// check if class implements interfaces
		// @todo
		logger.warning("isInstanceOf: does not check for interfaces yet : not implemented");
	}
	return false;
}

bool Class::isInterface() const {
	return _isInterface;
}
bool Class::isAbstract() const {
	return _isAbstract;
}
bool Class::hasSuperClass() const {
	if (_fullname == "java.lang.Object") {
		return false;  // java.lang.Object has no super class
	}
	return _hasSuperClass;
}

bool Class::hasMethod(const std::string& name_, const std::string& descriptor_) const {
	auto sig = name_ + descriptor_;
	auto it = _methods.find(sig);
	if (it != _methods.end()) {
		return true;
	}
	return false;
}

bool Class::isMethodOverloaded(const std::string& name_) const {
	int count = 0;
	for (const auto& [key, method] : _methods) {
		if (method->getName() == name_) {
			count++;
			if (count > 1) {
				return true;
			}
		}
	}
	return false;
}

Method& Class::getMethod(const std::string& name_, const std::string& descriptor_) {
	auto sig = name_ + descriptor_;
	auto it = _methods.find(sig);
	if (it != _methods.end()) {
		return *(it->second);
	}
	throw VmException("Method not found: {} {}", name_, descriptor_);
}

Method& Class::getMethod(uint32_t idx_) {
	if (idx_ >= _methods.size()) {
		throw std::out_of_range(fmt::format("Method index out of range: {}", idx_));
	}
	for (const auto& [name, method] : _methods) {
		logger.fdebug("method {}{} index {}", method->getName(), method->getSignature(), method->getIndex());
		if (method->getIndex() == idx_) {
			logger.fok("Found method {}{} at index {}", method->getName(), method->getSignature(), idx_);
		}
	}
	auto it = _methods.begin();
	std::advance(it, idx_);
	return *(it->second);
}

Field& Class::getField(const std::string& name_) {
	auto it = _fields.find(name_);
	if (it != _fields.end()) {
		return *(it->second);
	}
	throw VmException("Field not found: {}", name_);
}

Field& Class::getField(uint32_t idx_) {
	if (idx_ >= _fields.size()) {
		throw std::out_of_range(fmt::format("Field index out of range: {}", idx_));
	}
	auto it = _fields.begin();
	std::advance(it, idx_);
	return *(it->second);
}

std::vector<std::string> Class::getFieldList() const {
	std::vector<std::string> fieldList;
	for (const auto& [name, field] : _fields) {
		fieldList.push_back(name);
	}
	return fieldList;
}

Class& Class::getSuperClass() const {
	if (hasSuperClass()) {
		return _classloader.getOrLoad(_superClassname);
	}
	throw VmException("Class {} has no super class", getFullname());
}

std::string Class::getSuperClassname() const {
	return _superClassname;
}

bool Class::isExternal() const {
	if (isAbstract()) {
		return false;
	}
	if (isInterface()) {
		return false;
	}
	for (const auto& [key, method] : _methods) {
		if (method->hasBytecode()) {
			return false;
		}
	}
	return true;
}

void Class::debug() const {
	logger.fdebug("Class: {}", getFullname());
	/*for (auto& method : _class.methods()) {
	    logger.debug("Bytecode for method " + method.name() + get_method_descriptor(method) + ":");
	    std::ostringstream oss;
	    size_t count = 0;
	    for (const auto& byte : method.bytecode()) {
	        oss << fmt::format(" {:02x}", byte);
	        count++;
	        if (count % 32 == 0) {
	            logger.debug(oss.str());
	            oss.str("");
	            oss.clear();
	        }
	    }
	    if (!oss.str().empty()) {
	        logger.debug(oss.str());
	    }
	}*/
}