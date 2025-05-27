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

Method::Method(Class& class_, const std::string& name_, const std::string& signature_) : _class(class_) {
	_name = name_;
	_signature = signature_;

	_nbRegisters = 0;
	_index = 0;
	_bytecode = std::vector<uint8_t>();

	_isPublic = false;
	_isProtected = false;
	_isPrivate = false;
	_isFinal = false;
	_isStatic = false;
	_isAbstract = false;
	_isNative = false;
	_isVirtual = false;
}

Method::Method(Class& class_, const LIEF::DEX::Method& method_) : _class(class_) {
	_name = method_.name();
	_signature = get_method_descriptor(method_);

	_nbRegisters = method_.code_info().nb_registers();
	_index = method_.index();
	_bytecode = method_.bytecode();

	_isPublic = method_.has(LIEF::DEX::ACC_PUBLIC);
	_isProtected = method_.has(LIEF::DEX::ACC_PROTECTED);
	_isPrivate = method_.has(LIEF::DEX::ACC_PRIVATE);
	_isFinal = method_.has(LIEF::DEX::ACC_FINAL);
	_isStatic = method_.has(LIEF::DEX::ACC_STATIC);
	_isAbstract = method_.has(LIEF::DEX::ACC_ABSTRACT);
	_isNative = method_.has(LIEF::DEX::ACC_NATIVE);
	_isVirtual = method_.is_virtual();

	for (const auto& exc : method_.code_info().exceptions()) {
		_trycatch_items.push_back({exc.start_addr, exc.insn_count, exc.handlers, exc.catch_all_addr});
	}
}

Class& Method::getClass() const {
	return _class;
}
std::string Method::getName() const {
	return _name;
}

std::string Method::getSignature() const {
	return _signature;
}

uint32_t Method::getNbRegisters() const {
	return _nbRegisters;
}

uint32_t Method::getIndex() const {
	return _index;
}

std::vector<std::pair<uint32_t, uint32_t>> Method::getExceptionHandler(uint16_t pc_, uint32_t& catchAllAddr_) const {
	uint16_t pc = pc_ >> 1;
	logger.debug(fmt::format("getExceptionHandler: pc={:x} size={}", pc, _trycatch_items.size()));
	for (const auto& exc : _trycatch_items) {
		logger.debug(fmt::format("{}: Exception handler: start_addr: {:x}, insn_count: {:x}, catch_all_addr: {:x}", getName(), exc.start_addr, exc.insn_count,
		                         exc.catch_all_addr));
		if (pc >= exc.start_addr && pc < exc.start_addr + exc.insn_count) {
			catchAllAddr_ = exc.catch_all_addr;
			return exc.handlers;
		}
	}
	throw std::runtime_error(fmt::format("No exception handler found for pc: {}", pc_));
}

bool Method::hasBytecode() const {
	return _bytecode.empty() == false;
}

const uint8_t* const Method::getBytecode() const {
	return _bytecode.data();
}

bool Method::isStaticInitializer() const {
	return getName() == "<clinit>" && isStatic() && _signature == "()V";
}

bool Method::isConstructor() const {
	return getName() == "<init>" && _signature.back() == 'V' && !_class.isInterface();
}

bool Method::isStatic() const {
	return _isStatic;
}

bool Method::isPublic() const {
	return _isPublic;
}

bool Method::isPrivate() const {
	return _isPrivate;
}

bool Method::isFinal() const {
	return _isFinal;
}

bool Method::isAbstract() const {
	return _isAbstract;
}

bool Method::isNative() const {
	return _isNative;
}

bool Method::isVirtual() const {
	return _isVirtual;
}
