#include "method.hpp"

#include <fmt/format.h>

#include <LIEF/DEX/CodeInfo.hpp>
#include <LIEF/DEX/Method.hpp>
#include <LIEF/DEX/enums.hpp>
#include <sstream>

#include "class.hpp"
#include "system/logger.hpp"
#include "utils.hpp"

using namespace sandvik;

Method::Method(Class& class_, const LIEF::DEX::Method& method_) : _class(class_), _method(method_) {
}
Method::Method(const Method& other) : _class(other._class), _method(other._method) {
}
Class& Method::getClass() const {
	return _class;
}

std::string Method::getName() const {
	return _method.name();
}

std::string Method::getSignature() const {
	return get_method_descriptor(_method);
}

uint32_t Method::getNbRegisters() const {
	auto& c = _method.code_info();
	return c.nb_registers();
}

const uint8_t* const Method::getBytecode() const {
	return _method.bytecode().data();
}

bool Method::isStaticInitializer() const {
	return getName() == "<clinit>" && isStatic() && get_method_descriptor(_method) == "()V";
}

bool Method::isConstructor() const {
	return getName() == "<init>" && get_method_descriptor(_method).back() == 'V' && !_class.isInterface();
}

bool Method::isPublic() const {
	return _method.has(LIEF::DEX::ACC_PUBLIC);
}

bool Method::isPrivate() const {
	return _method.has(LIEF::DEX::ACC_PRIVATE);
}

bool Method::isProtected() const {
	return _method.has(LIEF::DEX::ACC_PROTECTED);
}

bool Method::isStatic() const {
	return _method.has(LIEF::DEX::ACC_STATIC);
}

bool Method::isFinal() const {
	return _method.has(LIEF::DEX::ACC_FINAL);
}

bool Method::isAbstract() const {
	return _method.has(LIEF::DEX::ACC_ABSTRACT);
}

bool Method::isNative() const {
	return _method.has(LIEF::DEX::ACC_NATIVE);
}
bool Method::isVirtual() const {
	return _method.is_virtual();
}

void Method::debug() const {
	std::ostringstream oss;
	oss << _method;
	logger.debug(oss.str());
	/*logger.debug(fmt::format("Method: {}::{}", _class.getName(), getSignature()));
	// logger.debug(fmt::format("Bytecode: {:x}", _method.bytecode()));
	logger.debug(fmt::format("Code offset: {}", _method.code_offset()));
	logger.debug(fmt::format("Is static initializer: {}", isStaticInitializer()));
	logger.debug(fmt::format("Is constructor: {}", isConstructor()));
	logger.debug(fmt::format("Is public: {}", isPublic()));
	logger.debug(fmt::format("Is private: {}", isPrivate()));
	logger.debug(fmt::format("Is protected: {}", isProtected()));
	logger.debug(fmt::format("Is static: {}", isStatic()));
	logger.debug(fmt::format("Is final: {}", isFinal()));
	logger.debug(fmt::format("Is abstract: {}", isAbstract()));
	logger.debug(fmt::format("Is native: {}", isNative()));
	logger.debug(fmt::format("Is interface: {}", _class.isInterface()));
	logger.debug(fmt::format("Is virtual: {}", _method.is_virtual()));
	logger.debug(fmt::format("Is synthetic: {}", _method.has(LIEF::DEX::ACC_SYNTHETIC)));
	logger.debug(fmt::format("Is bridge: {}", _method.has(LIEF::DEX::ACC_BRIDGE)));
	logger.debug(fmt::format("Is strict: {}", _method.has(LIEF::DEX::ACC_STRICT)));
	logger.debug(fmt::format("Is volatile: {}", _method.has(LIEF::DEX::ACC_VOLATILE)));
	logger.debug(fmt::format("Is transient: {}", _method.has(LIEF::DEX::ACC_TRANSIENT)));
	logger.debug(fmt::format("Is annotation: {}", _method.has(LIEF::DEX::ACC_ANNOTATION)));
	logger.debug(fmt::format("Is enum: {}", _method.has(LIEF::DEX::ACC_ENUM)));
	logger.debug(fmt::format("Is declared synchronized: {}", _method.has(LIEF::DEX::ACC_DECLARED_SYNCHRONIZED)));
	logger.debug(fmt::format("Is varargs: {}", _method.has(LIEF::DEX::ACC_VARARGS)));*/
}