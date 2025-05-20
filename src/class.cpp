#include "class.hpp"

#include <fmt/format.h>

#include <LIEF/DEX/Class.hpp>
#include <LIEF/DEX/Field.hpp>
#include <LIEF/DEX/Method.hpp>
#include <LIEF/DEX/Prototype.hpp>
#include <LIEF/DEX/Type.hpp>
#include <sstream>

#include "field.hpp"
#include "method.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "utils.hpp"

using namespace sandvik;

Class::Class(const uint32_t dexIdx_, const LIEF::DEX::Class& class_) : _dexIdx(dexIdx_), _class(class_) {
}

Class::Class(const Class& other) : _dexIdx(other._dexIdx), _class(other._class) {
	// Deep copy of methods
	for (const auto& [key, method] : other._methods) {
		_methods.emplace(key, std::make_unique<Method>(*method));
	}

	// Deep copy of fields
	for (const auto& [key, field] : other._fields) {
		_fields.emplace(key, std::make_unique<Field>(*field));
	}
}

uint32_t Class::getDexIdx() const {
	return _dexIdx;
}

std::string Class::getName() const {
	return _class.name();
}

std::string Class::getFullname() const {
	return _class.pretty_name();
}

bool Class::isExternal() const {
	if (isAbstract()) {
		return false;
	}
	for (const auto& method : _class.methods()) {
		if (!method.bytecode().empty()) {
			return false;
		}
	}
	return true;
}

bool Class::isInterface() const {
	return _class.has(LIEF::DEX::ACC_INTERFACE);
}
bool Class::isAbstract() const {
	return _class.has(LIEF::DEX::ACC_ABSTRACT);
}
bool Class::isPrimitive() const {
	return false;
}

bool Class::hasSuperClass() const {
	return _class.has_parent();
}

bool Class::inheritsFrom(Class& class_) const {
	logger.warning(fmt::format("class {} inherits from {} check is not implemented, always return true", class_.getFullname(), getFullname()));
	return true;
}

bool Class::isInstanceOf(std::shared_ptr<Object>& class_) const {
	if (class_->isNull()) {
		return false;
	}
	auto clazz = std::dynamic_pointer_cast<ObjectClass>(class_);
	if (!clazz) {
		auto str = std::dynamic_pointer_cast<StringObject>(class_);
		if (str && getFullname() == "java.lang.String") {
			return true;
		}
	} else {
		logger.debug(fmt::format("isInstanceOf: {}", clazz->getClass().getFullname()));
		if (clazz->getClass().getFullname() == getFullname()) {
			return true;
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
	for (const auto& method : _class.methods()) {
		auto descriptor = get_method_descriptor(method);
		if (method.name() == name_ && descriptor == descriptor_) {
			auto inserted = _methods.emplace(sig, std::make_unique<Method>(*this, method));
			return *(inserted.first->second);
		}
	}
	throw std::runtime_error("Method not found: " + name_ + " " + descriptor_);
}

Method& Class::getMethod(uint32_t idx_) {
	if (idx_ >= _class.methods().size()) {
		throw std::out_of_range("Method index out of range: " + std::to_string(idx_));
	}
	try {
		const auto& method = _class.methods()[idx_];
		auto descriptor = get_method_descriptor(method);
		auto sig = method.name() + descriptor;
		auto it = _methods.find(sig);
		if (it != _methods.end()) {
			return *(it->second);
		}
		auto inserted = _methods.emplace(sig, std::make_unique<Method>(*this, method));
		return *(inserted.first->second);
	} catch (std::exception& e) {
		throw std::runtime_error(fmt::format("Method not found for index={}", idx_));
	}
}
Field& Class::getField(const std::string& name_) {
	auto it = _fields.find(name_);
	if (it != _fields.end()) {
		return *(it->second);
	}
	for (const auto& field : _class.fields()) {
		if (field.name() == name_) {
			auto inserted = _fields.emplace(name_, std::make_unique<Field>(*this, field));
			return *(inserted.first->second);
		}
	}
	throw std::runtime_error("Field not found: " + name_);
}

Field& Class::getField(uint32_t idx_) {
	if (idx_ >= _class.fields().size()) {
		throw std::out_of_range("Field index out of range: " + std::to_string(idx_));
	}
	try {
		const auto& field = _class.fields()[idx_];
		auto it = _fields.find(field.name());
		if (it != _fields.end()) {
			return *(it->second);
		}
		auto inserted = _fields.emplace(field.name(), std::make_unique<Field>(*this, field));
		return *(inserted.first->second);
	} catch (std::exception& e) {
		throw std::runtime_error(fmt::format("Field not found for index={}", idx_));
	}
}

void Class::debug() const {
	for (auto& method : _class.methods()) {
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
	}
}