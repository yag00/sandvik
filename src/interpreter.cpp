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

#include "interpreter.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <functional>
#include <regex>

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "disassembler.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "frame.hpp"
#include "jni.hpp"
#include "jnihelper.hpp"
#include "jthread.hpp"
#include "method.hpp"
#include "native_call.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "trace.hpp"
#include "types.hpp"
#include "utils.hpp"
#include "vm.hpp"

using namespace sandvik;

Interpreter::Interpreter(JThread& rt_) : _rt(rt_) {
	_dispatch.fill(&Interpreter::invalidInstruction);

	_dispatch[0x00] = &Interpreter::nop;
	_dispatch[0x01] = &Interpreter::move;
	_dispatch[0x02] = &Interpreter::move_from16;
	_dispatch[0x03] = &Interpreter::move_16;
	_dispatch[0x04] = &Interpreter::move_wide;
	_dispatch[0x05] = &Interpreter::move_wide_from16;
	_dispatch[0x06] = &Interpreter::move_wide16;
	_dispatch[0x07] = &Interpreter::move_object;
	_dispatch[0x08] = &Interpreter::move_object_from16;
	_dispatch[0x09] = &Interpreter::move_object16;
	_dispatch[0x0A] = &Interpreter::move_result;
	_dispatch[0x0B] = &Interpreter::move_result_wide;
	_dispatch[0x0C] = &Interpreter::move_result_object;
	_dispatch[0x0D] = &Interpreter::move_exception;
	_dispatch[0x0E] = &Interpreter::return_void;
	_dispatch[0x0F] = &Interpreter::return_;
	_dispatch[0x10] = &Interpreter::return_wide;
	_dispatch[0x11] = &Interpreter::return_object;
	_dispatch[0x12] = &Interpreter::const_4;
	_dispatch[0x13] = &Interpreter::const_16;
	_dispatch[0x14] = &Interpreter::const_;
	_dispatch[0x15] = &Interpreter::const_high16;
	_dispatch[0x16] = &Interpreter::const_wide_16;
	_dispatch[0x17] = &Interpreter::const_wide_32;
	_dispatch[0x18] = &Interpreter::const_wide;
	_dispatch[0x19] = &Interpreter::const_wide_high16;
	_dispatch[0x1A] = &Interpreter::const_string;
	_dispatch[0x1B] = &Interpreter::const_string_jumbo;
	_dispatch[0x1C] = &Interpreter::const_class;
	_dispatch[0x1D] = &Interpreter::monitor_enter;
	_dispatch[0x1E] = &Interpreter::monitor_exit;
	_dispatch[0x1F] = &Interpreter::check_cast;
	_dispatch[0x20] = &Interpreter::instance_of;
	_dispatch[0x21] = &Interpreter::array_length;
	_dispatch[0x22] = &Interpreter::new_instance;
	_dispatch[0x23] = &Interpreter::new_array;
	_dispatch[0x24] = &Interpreter::filled_new_array;
	_dispatch[0x25] = &Interpreter::filled_new_array_range;
	_dispatch[0x26] = &Interpreter::fill_array_data;
	_dispatch[0x27] = &Interpreter::throw_;
	_dispatch[0x28] = &Interpreter::goto_;
	_dispatch[0x29] = &Interpreter::goto_16;
	_dispatch[0x2A] = &Interpreter::goto_32;
	_dispatch[0x2B] = &Interpreter::packed_switch;
	_dispatch[0x2C] = &Interpreter::sparse_switch;
	_dispatch[0x2D] = &Interpreter::cmpl_float;
	_dispatch[0x2E] = &Interpreter::cmpg_float;
	_dispatch[0x2F] = &Interpreter::cmpl_double;
	_dispatch[0x30] = &Interpreter::cmpg_double;
	_dispatch[0x31] = &Interpreter::cmp_long;
	_dispatch[0x32] = &Interpreter::if_eq;
	_dispatch[0x33] = &Interpreter::if_ne;
	_dispatch[0x34] = &Interpreter::if_lt;
	_dispatch[0x35] = &Interpreter::if_ge;
	_dispatch[0x36] = &Interpreter::if_gt;
	_dispatch[0x37] = &Interpreter::if_le;
	_dispatch[0x38] = &Interpreter::if_eqz;
	_dispatch[0x39] = &Interpreter::if_nez;
	_dispatch[0x3A] = &Interpreter::if_ltz;
	_dispatch[0x3B] = &Interpreter::if_gez;
	_dispatch[0x3C] = &Interpreter::if_gtz;
	_dispatch[0x3D] = &Interpreter::if_lez;
	// 0x3E ... 0x43 (unused)
	_dispatch[0x44] = &Interpreter::aget;
	_dispatch[0x45] = &Interpreter::aget_wide;
	_dispatch[0x46] = &Interpreter::aget_object;
	_dispatch[0x47] = &Interpreter::aget_boolean;
	_dispatch[0x48] = &Interpreter::aget_byte;
	_dispatch[0x49] = &Interpreter::aget_char;
	_dispatch[0x4A] = &Interpreter::aget_short;
	_dispatch[0x4B] = &Interpreter::aput;
	_dispatch[0x4C] = &Interpreter::aput_wide;
	_dispatch[0x4D] = &Interpreter::aput_object;
	_dispatch[0x4E] = &Interpreter::aput_boolean;
	_dispatch[0x4F] = &Interpreter::aput_byte;
	_dispatch[0x50] = &Interpreter::aput_char;
	_dispatch[0x51] = &Interpreter::aput_short;
	_dispatch[0x52] = &Interpreter::iget;
	_dispatch[0x53] = &Interpreter::iget_wide;
	_dispatch[0x54] = &Interpreter::iget_object;
	_dispatch[0x55] = &Interpreter::iget_boolean;
	_dispatch[0x56] = &Interpreter::iget_byte;
	_dispatch[0x57] = &Interpreter::iget_char;
	_dispatch[0x58] = &Interpreter::iget_short;
	_dispatch[0x59] = &Interpreter::iput;
	_dispatch[0x5A] = &Interpreter::iput_wide;
	_dispatch[0x5B] = &Interpreter::iput_object;
	_dispatch[0x5C] = &Interpreter::iput_boolean;
	_dispatch[0x5D] = &Interpreter::iput_byte;
	_dispatch[0x5E] = &Interpreter::iput_char;
	_dispatch[0x5F] = &Interpreter::iput_short;
	_dispatch[0x60] = &Interpreter::sget;
	_dispatch[0x61] = &Interpreter::sget_wide;
	_dispatch[0x62] = &Interpreter::sget_object;
	_dispatch[0x63] = &Interpreter::sget_boolean;
	_dispatch[0x64] = &Interpreter::sget_byte;
	_dispatch[0x65] = &Interpreter::sget_char;
	_dispatch[0x66] = &Interpreter::sget_short;
	_dispatch[0x67] = &Interpreter::sput;
	_dispatch[0x68] = &Interpreter::sput_wide;
	_dispatch[0x69] = &Interpreter::sput_object;
	_dispatch[0x6A] = &Interpreter::sput_boolean;
	_dispatch[0x6B] = &Interpreter::sput_byte;
	_dispatch[0x6C] = &Interpreter::sput_char;
	_dispatch[0x6D] = &Interpreter::sput_short;
	_dispatch[0x6E] = &Interpreter::invoke_virtual;
	_dispatch[0x6F] = &Interpreter::invoke_super;
	_dispatch[0x70] = &Interpreter::invoke_direct;
	_dispatch[0x71] = &Interpreter::invoke_static;
	_dispatch[0x72] = &Interpreter::invoke_interface;
	// 73 unused
	_dispatch[0x74] = &Interpreter::invoke_virtual_range;
	_dispatch[0x75] = &Interpreter::invoke_super_range;
	_dispatch[0x76] = &Interpreter::invoke_direct_range;
	_dispatch[0x77] = &Interpreter::invoke_static_range;
	_dispatch[0x78] = &Interpreter::invoke_interface_range;
	// 0x79 ... 0x7A (unused)
	_dispatch[0x7B] = &Interpreter::neg_int;
	_dispatch[0x7C] = &Interpreter::not_int;
	_dispatch[0x7D] = &Interpreter::neg_long;
	_dispatch[0x7E] = &Interpreter::not_long;
	_dispatch[0x7F] = &Interpreter::neg_float;
	_dispatch[0x80] = &Interpreter::neg_double;
	_dispatch[0x81] = &Interpreter::int_to_long;
	_dispatch[0x82] = &Interpreter::int_to_float;
	_dispatch[0x83] = &Interpreter::int_to_double;
	_dispatch[0x84] = &Interpreter::long_to_int;
	_dispatch[0x85] = &Interpreter::long_to_float;
	_dispatch[0x86] = &Interpreter::long_to_double;
	_dispatch[0x87] = &Interpreter::float_to_int;
	_dispatch[0x88] = &Interpreter::float_to_long;
	_dispatch[0x89] = &Interpreter::float_to_double;
	_dispatch[0x8A] = &Interpreter::double_to_int;
	_dispatch[0x8B] = &Interpreter::double_to_long;
	_dispatch[0x8C] = &Interpreter::double_to_float;
	_dispatch[0x8D] = &Interpreter::int_to_byte;
	_dispatch[0x8E] = &Interpreter::int_to_char;
	_dispatch[0x8F] = &Interpreter::int_to_short;
	_dispatch[0x90] = &Interpreter::add_int;
	_dispatch[0x91] = &Interpreter::sub_int;
	_dispatch[0x92] = &Interpreter::mul_int;
	_dispatch[0x93] = &Interpreter::div_int;
	_dispatch[0x94] = &Interpreter::rem_int;
	_dispatch[0x95] = &Interpreter::and_int;
	_dispatch[0x96] = &Interpreter::or_int;
	_dispatch[0x97] = &Interpreter::xor_int;
	_dispatch[0x98] = &Interpreter::shl_int;
	_dispatch[0x99] = &Interpreter::shr_int;
	_dispatch[0x9A] = &Interpreter::ushr_int;
	_dispatch[0x9B] = &Interpreter::add_long;
	_dispatch[0x9C] = &Interpreter::sub_long;
	_dispatch[0x9D] = &Interpreter::mul_long;
	_dispatch[0x9E] = &Interpreter::div_long;
	_dispatch[0x9F] = &Interpreter::rem_long;
	_dispatch[0xA0] = &Interpreter::and_long;
	_dispatch[0xA1] = &Interpreter::or_long;
	_dispatch[0xA2] = &Interpreter::xor_long;
	_dispatch[0xA3] = &Interpreter::shl_long;
	_dispatch[0xA4] = &Interpreter::shr_long;
	_dispatch[0xA5] = &Interpreter::ushr_long;
	_dispatch[0xA6] = &Interpreter::add_float;
	_dispatch[0xA7] = &Interpreter::sub_float;
	_dispatch[0xA8] = &Interpreter::mul_float;
	_dispatch[0xA9] = &Interpreter::div_float;
	_dispatch[0xAA] = &Interpreter::rem_float;
	_dispatch[0xAB] = &Interpreter::add_double;
	_dispatch[0xAC] = &Interpreter::sub_double;
	_dispatch[0xAD] = &Interpreter::mul_double;
	_dispatch[0xAE] = &Interpreter::div_double;
	_dispatch[0xAF] = &Interpreter::rem_double;
	_dispatch[0xB0] = &Interpreter::add_int_2addr;
	_dispatch[0xB1] = &Interpreter::sub_int_2addr;
	_dispatch[0xB2] = &Interpreter::mul_int_2addr;
	_dispatch[0xB3] = &Interpreter::div_int_2addr;
	_dispatch[0xB4] = &Interpreter::rem_int_2addr;
	_dispatch[0xB5] = &Interpreter::and_int_2addr;
	_dispatch[0xB6] = &Interpreter::or_int_2addr;
	_dispatch[0xB7] = &Interpreter::xor_int_2addr;
	_dispatch[0xB8] = &Interpreter::shl_int_2addr;
	_dispatch[0xB9] = &Interpreter::shr_int_2addr;
	_dispatch[0xBA] = &Interpreter::ushr_int_2addr;
	_dispatch[0xBB] = &Interpreter::add_long_2addr;
	_dispatch[0xBC] = &Interpreter::sub_long_2addr;
	_dispatch[0xBD] = &Interpreter::mul_long_2addr;
	_dispatch[0xBE] = &Interpreter::div_long_2addr;
	_dispatch[0xBF] = &Interpreter::rem_long_2addr;
	_dispatch[0xC0] = &Interpreter::and_long_2addr;
	_dispatch[0xC1] = &Interpreter::or_long_2addr;
	_dispatch[0xC2] = &Interpreter::xor_long_2addr;
	_dispatch[0xC3] = &Interpreter::shl_long_2addr;
	_dispatch[0xC4] = &Interpreter::shr_long_2addr;
	_dispatch[0xC5] = &Interpreter::ushr_long_2addr;
	_dispatch[0xC6] = &Interpreter::add_float_2addr;
	_dispatch[0xC7] = &Interpreter::sub_float_2addr;
	_dispatch[0xC8] = &Interpreter::mul_float_2addr;
	_dispatch[0xC9] = &Interpreter::div_float_2addr;
	_dispatch[0xCA] = &Interpreter::rem_float_2addr;
	_dispatch[0xCB] = &Interpreter::add_double_2addr;
	_dispatch[0xCC] = &Interpreter::sub_double_2addr;
	_dispatch[0xCD] = &Interpreter::mul_double_2addr;
	_dispatch[0xCE] = &Interpreter::div_double_2addr;
	_dispatch[0xCF] = &Interpreter::rem_double_2addr;
	_dispatch[0xD0] = &Interpreter::add_int_lit16;
	_dispatch[0xD1] = &Interpreter::rsub_int_lit16;
	_dispatch[0xD2] = &Interpreter::mul_int_lit16;
	_dispatch[0xD3] = &Interpreter::div_int_lit16;
	_dispatch[0xD4] = &Interpreter::rem_int_lit16;
	_dispatch[0xD5] = &Interpreter::and_int_lit16;
	_dispatch[0xD6] = &Interpreter::or_int_lit16;
	_dispatch[0xD7] = &Interpreter::xor_int_lit16;
	_dispatch[0xD8] = &Interpreter::add_int_lit8;
	_dispatch[0xD9] = &Interpreter::rsub_int_lit8;
	_dispatch[0xDA] = &Interpreter::mul_int_lit8;
	_dispatch[0xDB] = &Interpreter::div_int_lit8;
	_dispatch[0xDC] = &Interpreter::rem_int_lit8;
	_dispatch[0xDD] = &Interpreter::and_int_lit8;
	_dispatch[0xDE] = &Interpreter::or_int_lit8;
	_dispatch[0xDF] = &Interpreter::xor_int_lit8;
	_dispatch[0xE0] = &Interpreter::shl_int_lit8;
	_dispatch[0xE1] = &Interpreter::shr_int_lit8;
	_dispatch[0xE2] = &Interpreter::ushr_int_lit8;
	// 0xE3 ... 0xFF (unused)
}

Interpreter::~Interpreter() {
}

void Interpreter::invalidInstruction(const uint8_t* operand_) {
	throw VmException("Invalid instruction!");
}

void Interpreter::execute() {
	auto& frame = _rt.currentFrame();
	const auto& method = frame.getMethod();
	auto code = method.getBytecode();
	if (code == nullptr) {
		throw VmException("Method {}::{}{} has no bytecode!", method.getClass().getFullname(), method.getName(), method.getSignature());
	}
	if (frame.pc() >= method.getBytecodeSize()) {
		throw VmException("Current frame {}::{}{} has invalid pc: {}", method.getClass().getFullname(), method.getName(), method.getSignature(), frame.pc());
	}
	auto bytecode = code + frame.pc();
	if (trace.isInstructionTraceEnabled()) {
		trace.logInstruction(frame.pc(), fmt::format("{}::{}{}", method.getClass().getFullname(), method.getName(), method.getSignature()), bytecode);
	}
	frame.pc()++;
	try {
		(this->*_dispatch[*bytecode])(bytecode + 1);
	} catch (JavaException& e) {
		auto exctype = e.getExceptionType();
		logger.fdebug("handling exception {} ({}) in method {}::{}{}", exctype, e.what(), method.getClass().getFullname(), method.getName(),
		              method.getSignature());
		auto exc = Object::make(_rt.getClassLoader().getOrLoad(exctype));
		exc->setField("detailMessage", Object::make(_rt.getClassLoader(), e.getMessage()));
		handleException(exc);
	}
}

void Interpreter::executeClinit(Class& class_) const {
	auto hasInitializeMethod = class_.hasMethod("initializeSystemClass", "()V");
	auto hasClinitMethod = class_.hasMethod("<clinit>", "()V");
	if (!hasInitializeMethod && !hasClinitMethod) {
		// Nothing to do
		return;
	}
	logger.fdebug("Executing <clinit> for class {}", class_.getFullname());
	// lock the class to prevent concurrent initialization
	class_.monitorEnter();
	// auto unlock the class at the end of the function
	struct MonitorGuard {
			Class& class_;
			explicit MonitorGuard(Class& class__) : class_(class__) {
			}
			~MonitorGuard() {
				class_.monitorExit();
			}
			MonitorGuard(const MonitorGuard&) = delete;
			MonitorGuard& operator=(const MonitorGuard&) = delete;
	} guard{class_};

	if (class_.isStaticInitialized()) {
		return;
	}

	Method* initializeSystemClass = nullptr;
	if (hasInitializeMethod) {
		initializeSystemClass = &class_.getMethod("initializeSystemClass", "()V");
	}
	Method* clinitMethod = nullptr;
	if (hasClinitMethod) {
		clinitMethod = &class_.getMethod("<clinit>", "()V");
		if (clinitMethod->isNative()) {
			throw VmException("Native <clinit> method for class {} is not supported!", class_.getFullname());
		}
		if (!clinitMethod->hasBytecode()) {
			throw VmException("<clinit> method for class {} has no bytecode!", class_.getFullname());
		}
	}
	class_.setStaticInitialized();

	// Create a new "fake" thread to execute the <clinit> method (thread will be run in the current thread)
	auto& currentThread = _rt.vm().currentThread();
	auto& thread = currentThread.newChild(fmt::format("{}.{}", class_.getFullname(), "<clinit>"));
	if (initializeSystemClass != nullptr) {
		// push new frame with initializeSystemClass method (should be executed after <clinit>)
		thread.newFrame(*initializeSystemClass);
	}
	if (clinitMethod != nullptr) {
		// push new frame with <clinit> method
		thread.newFrame(*clinitMethod);
	}
	thread.runInCurrentThread();
	currentThread.popChild();
	logger.fdebug("Executing <clinit> for class {} OK", class_.getFullname());
}

void Interpreter::executeNativeMethod(const Method& method_, const std::vector<ObjectRef>& args_) {
	// Construct the JNI symbol name
	std::string symbolName = "Java_" + std::regex_replace(method_.getClass().getFullname(), std::regex("\\."), "_") + "_" + method_.getName();
	if (method_.isOverload()) {
		symbolName += "__" + JNIHelper::mangleMethodSignature(method_.getSignature());
	}
	logger.fdebug("call native jni function {}", symbolName);
	void* symbol = _rt.vm().findNativeSymbol(symbolName);
	if (!symbol) {
		throw VmException("Native method {} is not available!", symbolName);
	}

	const auto& desc = method_.getSignature();
	std::regex descriptorRegex(R"(\((.*?)\)(.*))");
	std::smatch match;
	if (!std::regex_match(desc, match, descriptorRegex)) {
		throw VmException("Invalid method descriptor: {}", desc);
	}
	std::string params = match[1];
	std::string returnType = match[2];
	logger.fdebug("Executing {}.{}{} -> native function {}@{:#x}", method_.getClass().getFullname(), method_.getName(), method_.getSignature(), symbolName,
	              (uintptr_t)symbol);
	ObjectRef staticClass = nullptr;
	if (method_.isStatic()) {
		staticClass = Object::make(method_.getClass());
	}
	auto ret = NativeCallHelper::invoke(symbol, _rt.vm().getJNIEnv(), args_, returnType, params, method_.isStatic(), staticClass);
	if (returnType != "V") {
		_rt.currentFrame().setReturnObject(ret);
	}
}

Method* Interpreter::resolveInterfaceHierarchyMethod(Class& interfaceClass_, const std::string& methodname_, const std::string& signature_,
                                                     std::unordered_set<std::string>& visited_) const {
	if (!visited_.insert(interfaceClass_.getFullname()).second) {
		return nullptr;
	}

	if (auto* method = interfaceClass_.findMethodOrNull(methodname_, signature_)) {
		if (!method->isStatic()) {
			return method;
		}
	}

	auto& classloader = _rt.getClassLoader();
	for (const auto& parentInterfaceName : interfaceClass_.getInterfaces()) {
		auto& parentInterface = classloader.getOrLoad(parentInterfaceName);
		if (!parentInterface.isStaticInitialized()) {
			executeClinit(parentInterface);
		}
		if (auto* method = resolveInterfaceHierarchyMethod(parentInterface, methodname_, signature_, visited_)) {
			return method;
		}
	}

	return nullptr;
}

Method* Interpreter::resolveInterfaceMethod(Class& instance_, const std::string& ifclassname_, const std::string& methodname_,
                                            const std::string& signature_) const {
	auto& classloader = _rt.getClassLoader();

	Class* current = &instance_;
	while (current) {
		if (auto* method = current->findMethodOrNull(methodname_, signature_)) {
			if (!method->isStatic()) {
				return method;
			}
		}

		if (current->hasSuperClass()) {
			current = &classloader.getOrLoad(current->getSuperClassname());
			if (!current->isStaticInitialized()) {
				executeClinit(*current);
			}
		} else {
			current = nullptr;
		}
	}

	auto& iface = classloader.getOrLoad(ifclassname_);

	if (!iface.isStaticInitialized()) {
		executeClinit(iface);
	}

	std::unordered_set<std::string> visited;
	return resolveInterfaceHierarchyMethod(iface, methodname_, signature_, visited);
}

void Interpreter::handleException(ObjectRef exception_) {
	if (!exception_->isClass()) {
		throw VmException("throw operand is not an object!");
	}

	auto detailMessage = exception_->getField("detailMessage");
	std::string msg = detailMessage->isString() ? detailMessage->str() : "";

	while (true) {
		if (_rt.stackDepth() == 0) {
			// uncaught exception
			logger.fdebug("Unhandled exception {} : {}", exception_->getClass().getFullname(), msg);
			throw JavaException(exception_->getClass().getFullname(), msg);
		}
		auto& frame = _rt.currentFrame();
		// do not re-handle same exception inside the same frame
		if (frame.isHandlingException(exception_)) {
			_rt.popFrame();
			continue;
		}
		try {
			const auto& method = frame.getMethod();
			uint32_t catchAllAddrress = 0;
			auto handlers = method.getExceptionHandler(frame.pc() - 1, catchAllAddrress);
			for (auto& exc : handlers) {
				const auto& exceptionType = _rt.getClassLoader().resolveClass(frame.getDexIdx(), exc.first);
				if (exceptionType.isInstanceOf(exception_)) {
					logger.fdebug("Catch exception {} at {:x}", exceptionType.getName(), exc.second);
					frame.pc() = exc.second << 1;
					frame.setException(exception_);
					return;
				}
			}
			if (catchAllAddrress != 0) {
				logger.fdebug("Catch all exception at {:x}", catchAllAddrress);
				frame.pc() = catchAllAddrress << 1;
				frame.setException(exception_);
				return;
			}
		} catch (const std::exception& e) {
			logger.fdebug("Exception handling failed: {}", e.what());
		}
		// No handler found - propagate to caller
		if (_rt.stackDepth() > 0) {
			_rt.popFrame();
		}
	}
}

std::vector<ObjectRef> Interpreter::getInvokeMethodArgs(const uint8_t* operand_) const {
	auto& frame = _rt.currentFrame();
	const uint8_t vA = (operand_[0] >> 4) & 0x0F;  // Number of registers (A)
	uint8_t vC = (vA > 0) ? operand_[3] & 0x0F : 0;
	uint8_t vD = (vA > 0) ? (operand_[3] >> 4) & 0x0F : 0;
	uint8_t vE = (vA > 1) ? operand_[4] & 0x0F : 0;
	uint8_t vF = (vA > 1) ? (operand_[4] >> 4) & 0x0F : 0;
	uint8_t vG = (vA > 2) ? operand_[0] & 0x0F : 0;

	std::vector<uint8_t> regs = {vC, vD, vE, vF, vG};
	std::vector<ObjectRef> args{};
	for (uint8_t i = 0; i < vA; ++i) {
		auto obj = frame.getObjRegister(regs[i]);
		args.push_back(obj);
	}
	return args;
}

bool Interpreter::tryRedirectToStringFactory(const Method& method, const std::vector<ObjectRef>& args, uint32_t thisRegIdx) {
	// Check if the method is a constructor of java.lang.String
	if (method.getName() != "<init>" || method.getClass().getFullname() != "java.lang.String") {
		return false;
	}
	// Search for a matching signature in the STRING_FACTORY_MAPPINGS
	const StringFactoryMapping* mapping = nullptr;
	for (const auto& entry : STRING_FACTORY_MAPPINGS) {
		if (entry.initSig == method.getSignature()) {
			mapping = &entry;
			break;
		}
	}
	logger.fdebug("Redirecting String constructor {} to StringFactory method {}", method.getSignature(), mapping ? mapping->targetSig : "none");
	// If no mapping is found, return false to fallback on the standard behavior
	if (!mapping) {
		return false;
	}
	// Prepare the arguments for the StringFactory method, excluding the 'this' reference
	std::vector<ObjectRef> factoryArgs(args.begin() + 1, args.end());
	if (mapping->initSig == "([CII)V") {
		factoryArgs = {factoryArgs[1], factoryArgs[2], factoryArgs[0]};
	}
	// Load the StringFactory class and retrieve the corresponding method
	auto& sfClass = _rt.getClassLoader().getOrLoad("Ljava/lang/StringFactory;");
	auto& sfMethod = sfClass.getMethod(std::string(mapping->targetName), std::string(mapping->targetSig));
	// If the StringFactory method is native, execute it directly; otherwise, create a new frame for it
	if (sfMethod.isNative()) {
		executeNativeMethod(sfMethod, factoryArgs);
		auto& frame = _rt.currentFrame();
		sandvik::ObjectRef newString = frame.getReturnObject();
		frame.setObjRegister(thisRegIdx, newString);
	} else {
		auto& newframe = _rt.newFrame(sfMethod);
		for (size_t i = 0; i < factoryArgs.size(); ++i) {
			uint32_t regIdx = sfMethod.getNbRegisters() - factoryArgs.size() + i;
			newframe.setObjRegister(regIdx, factoryArgs[i]);
		}
		// Store the 'this' register index for the new frame to set the return value later
		_stringCtorFixups[&newframe] = thisRegIdx;
	}
	return true;
}

void Interpreter::registerFinalizerIfNeeded(Class& cls_, ObjectRef obj_) const {
	// java.lang.Object.finalize() is a no-op; only classes that actually
	// override it need to be tracked by java.lang.ref.FinalizerReference,
	// mirroring what ART's native allocator does automatically on real
	// Android. We only register when THIS class (not an inherited one)
	// declares finalize() -- walking the hierarchy would double-register
	// objects whose finalize() comes from a finalizable superclass, since
	// the superclass's own instances already get registered when built.
	if (cls_.getFullname() == "java.lang.Object") {
		return;
	}
	static const std::unordered_set<std::string> excluded = {
	    "java.lang.ref.FinalizerReference", "java.lang.ref.Reference",     "java.lang.ref.ReferenceQueue",
	    "java.lang.ref.WeakReference",      "java.lang.ref.SoftReference", "java.lang.ref.PhantomReference",
	};
	if (excluded.count(cls_.getFullname())) {
		return;
	}
	if (!cls_.hasMethod("finalize", "()V")) {
		return;
	}
	auto& classloader = _rt.getClassLoader();
	auto& frClass = classloader.getOrLoad("java.lang.ref.FinalizerReference");
	auto& addMethod = frClass.getMethod("add", "(Ljava/lang/Object;)V");

	// Create a new "fake" thread to execute the add() method (thread will be run in the current thread)
	auto& currentThread = _rt.vm().currentThread();
	auto& thread = currentThread.newChild(fmt::format("{}.{}", frClass.getFullname(), "add"));
	auto& newframe = thread.newFrame(addMethod);
	// Single Object param, static method
	auto regidx = newframe.getMethod().getNbRegisters() - 1;
	newframe.setObjRegister(regidx, obj_);
	thread.runInCurrentThread();
	currentThread.popChild();
}

// nop
void Interpreter::nop(const uint8_t* operand_) {
	// No operation
}
// move vA, vB
void Interpreter::move(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	frame.setIntRegister(dest, frame.getIntRegister(src));
	frame.pc()++;
}
// move/from16 vAA, vBBBB
void Interpreter::move_from16(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t src = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	frame.setIntRegister(dest, frame.getIntRegister(src));
	frame.pc() += 3;
}
// move/16 vAAAA, vBBBB
void Interpreter::move_16(const uint8_t* operand_) {
	uint16_t dest = *(const uint16_t*)&operand_[0];
	uint16_t src = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	frame.setIntRegister(dest, frame.getIntRegister(src));
	frame.pc() += 4;
}
// move-wide vA, vB
void Interpreter::move_wide(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	frame.setIntRegister(dest, frame.getIntRegister(src));
	frame.setIntRegister(dest + 1, frame.getIntRegister(src + 1));
	frame.pc()++;
}
// move-wide/from16 vAA, vBBBB
void Interpreter::move_wide_from16(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t src = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	frame.setIntRegister(dest, frame.getIntRegister(src));
	frame.setIntRegister(dest + 1, frame.getIntRegister(src + 1));
	frame.pc() += 3;
}
// move-wide/16 vAAAA, vBBBB
void Interpreter::move_wide16(const uint8_t* operand_) {
	uint16_t dest = *(const uint16_t*)&operand_[0];
	uint16_t src = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	frame.setIntRegister(dest, frame.getIntRegister(src));
	frame.setIntRegister(dest + 1, frame.getIntRegister(src + 1));
	frame.pc() += 4;
}
// move-object vA, vB
void Interpreter::move_object(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	frame.setObjRegister(dest, frame.getObjRegister(src));
	frame.pc()++;
}
// move-object/from16 vAA, vBBBB
void Interpreter::move_object_from16(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t src = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	frame.setObjRegister(dest, frame.getObjRegister(src));
	frame.pc() += 3;
}
// move-object/16 vAAAA, vBBBB
void Interpreter::move_object16(const uint8_t* operand_) {
	uint16_t dest = *(const uint16_t*)&operand_[0];
	uint16_t src = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	frame.setObjRegister(dest, frame.getObjRegister(src));
	frame.pc() += 4;
}
//  move-result vAA
void Interpreter::move_result(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	auto& frame = _rt.currentFrame();
	frame.setIntRegister(dest, frame.getReturnValue());
	frame.pc() += 1;
}
// move-result-wide vAA
void Interpreter::move_result_wide(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	auto& frame = _rt.currentFrame();
	auto v = frame.getReturnDoubleValue();
	frame.setLongRegister(dest, v);
	frame.pc() += 1;
}
// move-result-object vAA
void Interpreter::move_result_object(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	auto& frame = _rt.currentFrame();
	frame.setObjRegister(dest, frame.getReturnObject());
	frame.pc() += 1;
}
// move-exception vAA
void Interpreter::move_exception(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	auto& frame = _rt.currentFrame();
	frame.setObjRegister(dest, frame.getException());
	frame.pc() += 1;
}
// return-void
void Interpreter::return_void(const uint8_t* operand_) {
	_rt.popFrame();
}
// return vAA
void Interpreter::return_(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	auto ret = _rt.currentFrame().getIntRegister(dest);
	_rt.popFrame();
	if (_rt.end()) {
		// main method return
		_rt.setReturnValue(ret);
		return;
	} else {
		_rt.currentFrame().setReturnValue(ret);
	}
}
// return-wide vAA
void Interpreter::return_wide(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	auto ret = _rt.currentFrame().getLongRegister(dest);
	_rt.popFrame();
	if (_rt.end()) {
		// main method return
		_rt.setReturnDoubleValue(ret);
		return;
	} else {
		_rt.currentFrame().setReturnDoubleValue(ret);
	}
}
// return-object vAA
void Interpreter::return_object(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	auto ret = _rt.currentFrame().getObjRegister(dest);
	Frame* returningFrame = &_rt.currentFrame();

	// Check if there is a fixup for the returning frame (used for String constructor redirection)
	auto fixupIt = _stringCtorFixups.find(returningFrame);
	bool hasFixup = (fixupIt != _stringCtorFixups.end());
	uint32_t fixupRegIdx = hasFixup ? fixupIt->second : 0;
	if (hasFixup) {
		_stringCtorFixups.erase(fixupIt);
	}

	_rt.popFrame();
	if (_rt.end()) {
		// main method return
		_rt.setReturnObject(ret);
		return;
	}
	if (hasFixup) {
		_rt.currentFrame().setObjRegister(fixupRegIdx, ret);
	} else {
		_rt.currentFrame().setReturnObject(ret);
	}
}
// const/4 vA, #+B
void Interpreter::const_4(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	int8_t value = static_cast<int8_t>((operand_[0] >> 4) & 0x0F);
	if (value & 0x08) {
		value |= 0xF0;  // Sign extend the 4-bit value to 8 bits
	}
	auto& frame = _rt.currentFrame();
	frame.setIntRegister(dest, (int32_t)value);
	frame.pc()++;
}
// const/16 vAA, #+BBBB
void Interpreter::const_16(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	int16_t value = static_cast<int16_t>(*(const uint16_t*)&operand_[1]);
	auto& frame = _rt.currentFrame();
	frame.setIntRegister(dest, (int32_t)value);
	frame.pc() += 3;
}
// const vAA, #+BBBBBBBB
void Interpreter::const_(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	int32_t value = *(const int32_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	frame.setIntRegister(dest, value);
	frame.pc() += 5;
}
// const/high16 vAA, #+BBBB0000
void Interpreter::const_high16(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint32_t value = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	frame.setIntRegister(dest, value << 16);
	frame.pc() += 3;
}
// const-wide/16 vAA, #+BBBB
void Interpreter::const_wide_16(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	int16_t value = static_cast<int16_t>(*(const uint16_t*)&operand_[1]);
	auto& frame = _rt.currentFrame();
	frame.setLongRegister(dest, (int64_t)value);
	frame.pc() += 3;
}
// const-wide/32 vAA, #+BBBBBBBB
void Interpreter::const_wide_32(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	int32_t value = *(const int32_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	frame.setLongRegister(dest, value);
	frame.pc() += 5;
}
// const-wide vAA, #+BBBBBBBBBBBBBBBB
void Interpreter::const_wide(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	int64_t value = *(const int64_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	frame.setLongRegister(dest, value);
	frame.pc() += 9;
}
// const-wide/high16 vAA, #+BBBB000000000000
void Interpreter::const_wide_high16(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint64_t value = static_cast<uint64_t>(*(const uint16_t*)&operand_[1]) << 48;
	auto& frame = _rt.currentFrame();
	frame.setLongRegister(dest, value);
	frame.pc() += 3;
}
// const-string vAA, string@BBBB
void Interpreter::const_string(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t stringIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();
	auto str = classloader.resolveString(frame.getDexIdx(), stringIndex);
	frame.setObjRegister(dest, Object::make(classloader, str));
	frame.pc() += 3;
}
// const-string/jumbo vAA, string@BBBBBBBB
void Interpreter::const_string_jumbo(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint32_t stringIndex = *(const uint32_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();
	auto str = classloader.resolveString(frame.getDexIdx(), stringIndex);
	frame.setObjRegister(dest, Object::make(classloader, str));
	frame.pc() += 5;
}
// const-class vAA, type@BBBB
void Interpreter::const_class(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t typeIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();
	auto& cls = classloader.resolveClass(frame.getDexIdx(), typeIndex);
	frame.setObjRegister(dest, Object::makeConstClass(classloader, cls));
	frame.pc() += 3;
}
// monitor-enter vAA
void Interpreter::monitor_enter(const uint8_t* operand_) {
	uint8_t reg = operand_[0];
	auto& frame = _rt.currentFrame();
	auto obj = frame.getObjRegister(reg);
	if (obj->isNull()) {
		throw NullPointerException("monitor_enter on null object");
	}
	obj->monitorEnter();
	logger.fdebug("monitor enter on object {}", obj->toString());
	frame.pc()++;
}
// monitor-exit vAA
void Interpreter::monitor_exit(const uint8_t* operand_) {
	uint8_t reg = operand_[0];
	auto& frame = _rt.currentFrame();
	auto obj = frame.getObjRegister(reg);
	if (obj->isNull()) {
		throw NullPointerException("monitor_exit on null object");
	}
	logger.fdebug("monitor exit on object {}", obj->toString());
	obj->monitorExit();
	frame.pc()++;
}
// check-cast vAA, type@BBBB
void Interpreter::check_cast(const uint8_t* operand_) {
	uint8_t reg = operand_[0];
	uint16_t typeIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto obj = frame.getObjRegister(reg);

	// check-cast null is LEGAL in Java and Dalvik.
	if (obj->isNull()) {
		// null can be cast to anything
		frame.pc() += 3;
		return;
	}

	auto& classloader = _rt.getClassLoader();
	TYPES type = TYPES::UNKNOWN;
	std::string type_name = classloader.resolveType(frame.getDexIdx(), typeIndex, type);
	switch (type) {
		case TYPES::PRIMITIVE:
			// Primitive types: no casting needed, always valid
			logger.fdebug("@todo check-cast to primitive type {}", type_name);
			break;
		case TYPES::CLASS: {
			auto& targetClass = classloader.resolveClass(frame.getDexIdx(), typeIndex);
			if (!targetClass.isInstanceOf(obj)) {
				throw ClassCastException(fmt::format("Cannot cast object to {}", targetClass.getName()));
			}
			break;
		}
		case TYPES::ARRAY: {
			auto array = static_cast<ArrayRef>(obj);
			if (!array) {
				throw ClassCastException("Object is not an array");
			}
			size_t array_dims = 0;
			size_t pos = 0;
			while (pos < type_name.size() && type_name[pos] == '[') {
				array_dims++;
				pos++;
			}
			std::string element_type_name = type_name.substr(pos);
			logger.fdebug("Array type: {} dimensions, element type {}", array_dims, element_type_name);
			// check object has good dimensions
			if (array->getDimensions() != array_dims) {
				throw ClassCastException(fmt::format("Cannot cast array of {} dimensions to {}", array->getDimensions(), array_dims));
			}
			// check if element type is class or primitive
			if (element_type_name[0] == 'L') {
				auto classname = element_type_name.substr(1);  // remove 'L'
				classname.pop_back();                          // remove ';'
				auto& targetClass = classloader.getOrLoad(classname);
				if (!targetClass.isInstanceOf(array->getClassType())) {
					throw ClassCastException(fmt::format("Cannot cast array to {}", targetClass.getName()));
				}
			} else {
				// Primitive types: no casting needed, always valid
				logger.fdebug("@todo check-cast to array of primitive type {}", element_type_name);
			}
			break;
		}
		default:
			throw VmException("check-cast: Unsupported type {}", type_name);
	}
	frame.pc() += 3;
}
// instance-of vA, vB, type@CCCC
void Interpreter::instance_of(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	uint16_t typeIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto obj = frame.getObjRegister(src);
	auto& classloader = _rt.getClassLoader();
	auto& targetClass = classloader.resolveClass(frame.getDexIdx(), typeIndex);
	frame.setIntRegister(dest, targetClass.isInstanceOf(obj) ? 1 : 0);
	frame.pc() += 3;
}
// array-length vA, vB
void Interpreter::array_length(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	auto obj = frame.getObjRegister(src);
	if (obj->isNull()) {
		const auto& method = frame.getMethod();
		throw NullPointerException(
		    fmt::format("array_length on null object in {}.{}{}", method.getClass().getFullname(), method.getName(), method.getSignature()));
	}
	uint32_t length = obj->getArrayLength();
	frame.setIntRegister(dest, length);
	frame.pc()++;
}
// new-instance vAA, type@BBBB
void Interpreter::new_instance(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t typeIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();
	auto& cls = classloader.resolveClass(frame.getDexIdx(), typeIndex);

	if (cls.isAbstract() || cls.isInterface()) {
		throw InstantiationException(fmt::format("Cannot instantiate abstract class or interface: {}", cls.getName()));
	}
	if (!cls.isStaticInitialized()) {
		executeClinit(cls);
	}

	logger.fdebug("new {}", cls.getFullname());
	auto obj = Object::make(cls);
	frame.setObjRegister(dest, obj);

	registerFinalizerIfNeeded(cls, obj);
	frame.pc() += 3;
}
// new-array vA, vB, type@CCCC
void Interpreter::new_array(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	uint16_t typeIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();

	auto& classloader = _rt.getClassLoader();
	auto arrayType = classloader.resolveArray(frame.getDexIdx(), typeIndex);

	// Get the array size from the source register
	int32_t size = frame.getIntRegister(src);
	if (size < 0) {
		throw NegativeArraySizeException("new_array: Array size cannot be negative");
	}
	const auto& baseName = arrayType[0].first;
	// resolveArray reports the leaf base type ("int") plus how many extra levels of array nesting
	// remain beyond the immediate element (0 for a plain int[], 1 for the outer level of int[][],
	// created one dimension at a time by new-array, etc.) - not the leaf type's own dim(), which is
	// always 0 for a primitive/class item (see Dex::resolveArray).
	uint32_t extraDims = arrayType[0].second;
	const Class* elementType = nullptr;
	if (extraDims == 0) {
		elementType = &classloader.getOrLoad(baseName);
	} else {
		// This array's own element type is an array type in its own right (e.g. "[I" for the outer
		// level of int[][]), not the leaf type - build that descriptor explicitly.
		std::string baseDescriptor = sandvik::is_primitive_type(baseName) ? sandvik::get_primitive_descriptor(baseName)
		                                                                  : "L" + std::regex_replace(baseName, std::regex("\\."), "/") + ";";
		std::string componentDescriptor(extraDims, '[');
		componentDescriptor += baseDescriptor;
		elementType = &classloader.getOrLoad(componentDescriptor);
	}
	auto arrayObj = Array::make(*elementType, size);
	frame.setObjRegister(dest, arrayObj);
	frame.pc() += 3;
}
// filled-new-array {vD, vE, vF, vG, vA}, type@CCCC
void Interpreter::filled_new_array(const uint8_t* operand_) {
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto args = getInvokeMethodArgs(operand_);
	uint8_t count = (operand_[0] >> 4) & 0x0F;
	uint16_t typeIndex = *(const uint16_t*)&operand_[1];

	auto arrayType = classloader.resolveArray(frame.getDexIdx(), typeIndex);
	if (arrayType.empty()) {
		throw VmException("filled-new-array: cannot resolve array type for index {}", typeIndex);
	}
	if (count != args.size()) {
		throw VmException("filled-new-array: argument count mismatch (expected {}, got {})", count, args.size());
	}

	const auto& compClass = classloader.getOrLoad(arrayType[0].first);
	auto array = Array::make(compClass, count);
	for (uint8_t i = 0; i < args.size(); ++i) {
		array->setElement(i, args[i]);
	}
	frame.setReturnObject(array);
	frame.pc() += 5;
}
// filled-new-array/range {vCCCC .. vNNNN}, type@BBBB
void Interpreter::filled_new_array_range(const uint8_t* operand_) {
	// NOT TESTED: We have not found Java code generating this instruction yet due to recent d8 compiler.
	const uint8_t count = operand_[0];
	const uint16_t typeIndex = *reinterpret_cast<const uint16_t*>(&operand_[1]);
	const uint16_t startReg = *reinterpret_cast<const uint16_t*>(&operand_[3]);

	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto arrayType = classloader.resolveArray(frame.getDexIdx(), typeIndex);
	if (arrayType.empty()) {
		throw VmException("filled-new-array/range: cannot resolve array type for index {}", typeIndex);
	}

	std::vector<ObjectRef> args;
	args.reserve(count);
	for (uint8_t i = 0; i < count; ++i) {
		args.push_back(frame.getObjRegister(startReg + i));
	}

	const auto& compClass = classloader.getOrLoad(arrayType[0].first);
	auto array = Array::make(compClass, count);
	for (uint8_t i = 0; i < args.size(); ++i) {
		array->setElement(i, args[i]);
	}
	frame.setReturnObject(array);
	frame.pc() += 5;
}
// fill-array-data vAA, +BBBBBBBB
void Interpreter::fill_array_data(const uint8_t* operand_) {
	uint8_t reg = operand_[0];
	auto offset = *reinterpret_cast<const uint32_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	auto baseAddress = frame.pc() - 1;  // Adjust for the incremented PC
	auto data = reinterpret_cast<const uint16_t*>(frame.getMethod().getBytecode() + baseAddress + (offset << 1));

	uint16_t ident = data[0];
	if (ident != 0x0300) {  // Array-data identifier
		throw VmException("Invalid array-data identifier: 0x{:04x}", ident);
	}

	uint32_t elementSize = data[1];
	uint32_t elementCount = *(const uint32_t*)&data[2];
	auto arrayData = reinterpret_cast<const uint8_t*>(&data[4]);

	auto arrayObj = frame.getObjRegister(reg);
	if (arrayObj->isNull()) {
		throw NullPointerException("fill_array_data on null array object");
	}

	auto array = static_cast<ArrayRef>(arrayObj);
	if (!array) {
		throw ClassCastException("fill_array_data: Object is not an array");
	}

	if (array->getArrayLength() != elementCount) {
		throw ClassCastException("fill_array_data: Array length mismatch");
	}

	for (uint32_t i = 0; i < elementCount; ++i) {
		switch (elementSize) {
			case 1: {
				int8_t value = *reinterpret_cast<const int8_t*>(arrayData);
				array->setElement(i, Object::make(value));
				arrayData += 1;
				break;
			}
			case 2: {
				int16_t value = *reinterpret_cast<const int16_t*>(arrayData);
				array->setElement(i, Object::make(value));
				arrayData += 2;
				break;
			}
			case 4: {
				int32_t value = *reinterpret_cast<const int32_t*>(arrayData);
				array->setElement(i, Object::make(value));
				arrayData += 4;
				break;
			}
			case 8: {
				int64_t value = *reinterpret_cast<const int64_t*>(arrayData);
				array->setElement(i, Object::make(value));
				arrayData += 8;
				break;
			}
			default:
				throw ArrayStoreException(fmt::format("fill_array_data: Unsupported element size: {}", elementSize));
		}
	}
	frame.pc() += 5;
}
// throw vAA
void Interpreter::throw_(const uint8_t* operand_) {
	uint8_t reg = operand_[0];
	auto obj = _rt.currentFrame().getObjRegister(reg);
	if (obj->isNull()) {
		throw NullPointerException("throw on null object");
	}
	handleException(obj);
}
// goto +AA
void Interpreter::goto_(const uint8_t* operand_) {
	auto& frame = _rt.currentFrame();
	int8_t offset = static_cast<int8_t>(operand_[0]);
	frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
}
// goto/16 +AAAA
void Interpreter::goto_16(const uint8_t* operand_) {
	auto& frame = _rt.currentFrame();
	int16_t offset = *reinterpret_cast<const int16_t*>(&operand_[1]);
	frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
}
// goto/32 +AAAAAAAA
void Interpreter::goto_32(const uint8_t* operand_) {
	auto& frame = _rt.currentFrame();
	int32_t offset = *reinterpret_cast<const int32_t*>(&operand_[1]);
	frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
}
// packed-switch vAA, +BBBBBBBB
void Interpreter::packed_switch(const uint8_t* operand_) {
	uint8_t reg = operand_[0];
	uint32_t offset = *reinterpret_cast<const uint32_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	auto baseAddress = frame.pc() - 1;  // Adjust for the incremented PC
	auto switchData = reinterpret_cast<const uint16_t*>(frame.getMethod().getBytecode() + baseAddress + (offset << 1));

	uint16_t ident = switchData[0];
	if (ident != 0x0100) {  // Packed-switch identifier
		throw VmException("Invalid packed-switch identifier: 0x{:04x}", ident);
	}
	int32_t size = switchData[1];
	if (size <= 0) {
		throw ArrayIndexOutOfBoundsException("packed-switch: Invalid size in switch data");
	}

	auto base = *(const int32_t*)&switchData[2];
	auto targets = reinterpret_cast<const int32_t*>(&switchData[4]);
	if (!targets) {
		throw VmException("packed-switch: Invalid targets pointer");
	}

	int32_t value = frame.getIntRegister(reg);
	if (value >= base && value < base + size) {
		int32_t targetOffset = targets[value - base];
		frame.pc() = baseAddress + (targetOffset << 1);
	} else {
		frame.pc() += 5;  // Default case, skip the instruction
	}
}
// sparse-switch vAA, +BBBBBBBB
void Interpreter::sparse_switch(const uint8_t* operand_) {
	uint8_t reg = operand_[0];
	uint32_t offset = *reinterpret_cast<const uint32_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	auto baseAddress = frame.pc() - 1;  // Adjust for the incremented PC
	auto switchData = reinterpret_cast<const uint16_t*>(frame.getMethod().getBytecode() + baseAddress + (offset << 1));

	uint16_t ident = switchData[0];
	if (ident != 0x0200) {  // Sparse-switch identifier
		throw VmException("Invalid sparse-switch identifier: 0x{:04x}", ident);
	}
	int32_t size = switchData[1];
	if (size <= 0) {
		throw ArrayIndexOutOfBoundsException("sparse-switch: Invalid size in switch data");
	}

	auto keys = reinterpret_cast<const int32_t*>(&switchData[2]);
	auto targets = reinterpret_cast<const int32_t*>(&switchData[2 + size * 2]);

	int32_t value = frame.getIntRegister(reg);
	for (int32_t i = 0; i < size; ++i) {
		if (keys[i] == value) {
			int32_t targetOffset = targets[i];
			frame.pc() = baseAddress + (targetOffset << 1);
			return;
		}
	}

	// Default case, skip the instruction
	frame.pc() += 5;
}
// cmpl-float vAA, vBB, vCC
void Interpreter::cmpl_float(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	float value1 = frame.getFloatRegister(src1);
	float value2 = frame.getFloatRegister(src2);
	int32_t result = (value1 < value2) ? -1 : (value1 == value2 ? 0 : 1);
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// cmpg-float vAA, vBB, vCC
void Interpreter::cmpg_float(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	float value1 = frame.getFloatRegister(src1);
	float value2 = frame.getFloatRegister(src2);
	int32_t result = (value1 > value2) ? 1 : (value1 == value2 ? 0 : -1);
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// cmpl-double vAA, vBB, vCC
void Interpreter::cmpl_double(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	double value1 = frame.getDoubleRegister(src1);
	double value2 = frame.getDoubleRegister(src2);
	int32_t result = (value1 < value2) ? -1 : (value1 == value2 ? 0 : 1);
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// cmpg-double vAA, vBB, vCC
void Interpreter::cmpg_double(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	double value1 = frame.getDoubleRegister(src1);
	double value2 = frame.getDoubleRegister(src2);
	int32_t result = (value1 > value2) ? 1 : (value1 == value2 ? 0 : -1);
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// cmp-long vAA, vBB, vCC
void Interpreter::cmp_long(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int64_t value1 = frame.getLongRegister(src1);
	int64_t value2 = frame.getLongRegister(src2);
	int32_t result = (value1 < value2) ? -1 : (value1 == value2 ? 0 : 1);
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// if-eq vA, vB, +CCCC
void Interpreter::if_eq(const uint8_t* operand_) {
	uint8_t regA = operand_[0] & 0x0F;
	uint8_t regB = (operand_[0] >> 4) & 0x0F;
	int16_t offset = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	// obj comparison as if eq could be used for object equality
	auto objA = frame.getObjRegister(regA);
	auto objB = frame.getObjRegister(regB);
	if (objA->isNull() || objB->isNull()) {
		if (objA->isNull() && objB->isNull()) {
			frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
		} else {
			frame.pc() += 3;
		}
		return;
	}
	if (*objA == *objB) {
		frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
	} else {
		frame.pc() += 3;
	}
}
// if-ne vA, vB, +CCCC
void Interpreter::if_ne(const uint8_t* operand_) {
	uint8_t regA = operand_[0] & 0x0F;
	uint8_t regB = (operand_[0] >> 4) & 0x0F;
	int16_t offset = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	// obj comparison as if ne could be used for object equality
	auto objA = frame.getObjRegister(regA);
	auto objB = frame.getObjRegister(regB);
	if (objA->isNull() || objB->isNull()) {
		if (objA->isNull() && objB->isNull()) {
			frame.pc() += 3;
		} else {
			frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
		}
		return;
	}
	if (*objA != *objB) {
		frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
	} else {
		frame.pc() += 3;
	}
}
// if-lt vA, vB, +CCCC
void Interpreter::if_lt(const uint8_t* operand_) {
	uint8_t regA = operand_[0] & 0x0F;
	uint8_t regB = (operand_[0] >> 4) & 0x0F;
	int16_t offset = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	if (frame.getIntRegister(regA) < frame.getIntRegister(regB)) {
		frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
	} else {
		frame.pc() += 3;
	}
}
// if-ge vA, vB, +CCCC
void Interpreter::if_ge(const uint8_t* operand_) {
	uint8_t regA = operand_[0] & 0x0F;
	uint8_t regB = (operand_[0] >> 4) & 0x0F;
	int16_t offset = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	if (frame.getIntRegister(regA) >= frame.getIntRegister(regB)) {
		frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
	} else {
		frame.pc() += 3;
	}
}
// if-gt vA, vB, +CCCC
void Interpreter::if_gt(const uint8_t* operand_) {
	uint8_t regA = operand_[0] & 0x0F;
	uint8_t regB = (operand_[0] >> 4) & 0x0F;
	int16_t offset = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	if (frame.getIntRegister(regA) > frame.getIntRegister(regB)) {
		frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
	} else {
		frame.pc() += 3;
	}
}
// if-le vA, vB, +CCCC
void Interpreter::if_le(const uint8_t* operand_) {
	uint8_t regA = operand_[0] & 0x0F;
	uint8_t regB = (operand_[0] >> 4) & 0x0F;
	int16_t offset = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	if (frame.getIntRegister(regA) <= frame.getIntRegister(regB)) {
		frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
	} else {
		frame.pc() += 3;
	}
}
// if-eqz vAA, +BBBB
void Interpreter::if_eqz(const uint8_t* operand_) {
	uint8_t regA = operand_[0];
	int16_t offset = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	auto obj = frame.getObjRegister(regA);
	if (obj->isNumberObject()) {
		if (obj->getValue() == 0) {
			frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
		} else {
			frame.pc() += 3;
		}
	} else {
		if (obj->isNull()) {
			frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
		} else {
			frame.pc() += 3;
		}
	}
}
// if-nez vAA, +BBBB
void Interpreter::if_nez(const uint8_t* operand_) {
	uint8_t regA = operand_[0];
	int16_t offset = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	auto obj = frame.getObjRegister(regA);
	if (obj->isNumberObject()) {
		if (obj->getValue() != 0) {
			frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
		} else {
			frame.pc() += 3;
		}
	} else {
		if (!obj->isNull()) {
			frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
		} else {
			frame.pc() += 3;
		}
	}
}
// if-ltz vAA, +BBBB
void Interpreter::if_ltz(const uint8_t* operand_) {
	uint8_t regA = operand_[0];
	int16_t offset = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	if (frame.getIntRegister(regA) < 0) {
		frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
	} else {
		frame.pc() += 3;
	}
}
// if-gez vAA, +BBBB
void Interpreter::if_gez(const uint8_t* operand_) {
	uint8_t regA = operand_[0];
	int16_t offset = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	if (frame.getIntRegister(regA) >= 0) {
		frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
	} else {
		frame.pc() += 3;
	}
}
// if-gtz vAA, +BBBB
void Interpreter::if_gtz(const uint8_t* operand_) {
	uint8_t regA = operand_[0];
	int16_t offset = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	if (frame.getIntRegister(regA) > 0) {
		frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
	} else {
		frame.pc() += 3;
	}
}
// if-lez vAA, +BBBB
void Interpreter::if_lez(const uint8_t* operand_) {
	uint8_t regA = operand_[0];
	int16_t offset = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	if (frame.getIntRegister(regA) <= 0) {
		frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
	} else {
		frame.pc() += 3;
	}
}
// aget vAA, vBB, vCC
void Interpreter::aget(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t arrayReg = operand_[1];
	uint8_t indexReg = operand_[2];
	auto& frame = _rt.currentFrame();

	auto arrayObj = frame.getObjRegister(arrayReg);
	if (arrayObj->isNull()) {
		throw NullPointerException("aget on null array object");
	}
	auto array = static_cast<ArrayRef>(arrayObj);
	if (!array) {
		throw ClassCastException("aget: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aget: Array index out of bounds");
	}
	int32_t value = static_cast<int32_t>(static_cast<int64_t>(array->getRawElement(index)));
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// aget-wide vAA, vBB, vCC
void Interpreter::aget_wide(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t arrayReg = operand_[1];
	uint8_t indexReg = operand_[2];
	auto& frame = _rt.currentFrame();

	auto arrayObj = frame.getObjRegister(arrayReg);
	if (arrayObj->isNull()) {
		throw NullPointerException("aget-wide on null array object");
	}
	auto array = static_cast<ArrayRef>(arrayObj);
	if (!array) {
		throw ClassCastException("aget-wide: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aget-wide: Array index out of bounds");
	}
	int64_t value = static_cast<int64_t>(array->getRawElement(index));
	frame.setLongRegister(dest, value);
	frame.pc() += 3;
}
// aget-object vAA, vBB, vCC
void Interpreter::aget_object(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t arrayReg = operand_[1];
	uint8_t indexReg = operand_[2];
	auto& frame = _rt.currentFrame();

	auto arrayObj = frame.getObjRegister(arrayReg);
	if (arrayObj->isNull()) {
		throw NullPointerException("aget-object on null array object");
	}
	auto array = static_cast<ArrayRef>(arrayObj);
	if (!array) {
		throw ClassCastException("aget-object: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aget-object: Array index out of bounds");
	}
	auto value = array->getElement(index);
	frame.setObjRegister(dest, value);
	frame.pc() += 3;
}
// aget-boolean vAA, vBB, vCC
void Interpreter::aget_boolean(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t arrayReg = operand_[1];
	uint8_t indexReg = operand_[2];
	auto& frame = _rt.currentFrame();

	auto arrayObj = frame.getObjRegister(arrayReg);
	if (arrayObj->isNull()) {
		throw NullPointerException("aget-boolean on null array object");
	}
	auto array = static_cast<ArrayRef>(arrayObj);
	if (!array) {
		throw ClassCastException("aget-boolean: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aget-boolean: Array index out of bounds");
	}
	bool value = static_cast<uint8_t>(static_cast<int32_t>(static_cast<int64_t>(array->getRawElement(index)))) != 0;
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// aget-byte vAA, vBB, vCC
void Interpreter::aget_byte(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t arrayReg = operand_[1];
	uint8_t indexReg = operand_[2];
	auto& frame = _rt.currentFrame();

	auto arrayObj = frame.getObjRegister(arrayReg);
	if (arrayObj->isNull()) {
		throw NullPointerException("aget-byte on null array object");
	}
	auto array = static_cast<ArrayRef>(arrayObj);
	if (!array) {
		throw ClassCastException("aget-byte: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aget-byte: Array index out of bounds");
	}
	int8_t value = static_cast<int8_t>(static_cast<int32_t>(static_cast<int64_t>(array->getRawElement(index))));
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// aget-char vAA, vBB, vCC
void Interpreter::aget_char(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t arrayReg = operand_[1];
	uint8_t indexReg = operand_[2];
	auto& frame = _rt.currentFrame();

	auto arrayObj = frame.getObjRegister(arrayReg);
	if (arrayObj->isNull()) {
		throw NullPointerException("aget-char on null array object");
	}
	auto array = static_cast<ArrayRef>(arrayObj);
	if (!array) {
		throw ClassCastException("aget-char: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aget-char: Array index out of bounds");
	}
	uint16_t value = static_cast<uint16_t>(static_cast<int32_t>(static_cast<int64_t>(array->getRawElement(index))));
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// aget-short vAA, vBB, vCC
void Interpreter::aget_short(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t arrayReg = operand_[1];
	uint8_t indexReg = operand_[2];
	auto& frame = _rt.currentFrame();

	auto arrayObj = frame.getObjRegister(arrayReg);
	if (arrayObj->isNull()) {
		throw NullPointerException("aget-short on null array object");
	}
	auto array = static_cast<ArrayRef>(arrayObj);
	if (!array) {
		throw ClassCastException("aget-short: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aget-short: Array index out of bounds");
	}
	int16_t value = static_cast<int16_t>(static_cast<int32_t>(static_cast<int64_t>(array->getRawElement(index))));
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// aput vAA, vBB, vCC
void Interpreter::aput(const uint8_t* operand_) {
	uint8_t valueReg = operand_[0];
	uint8_t arrayReg = operand_[1];
	uint8_t indexReg = operand_[2];
	auto& frame = _rt.currentFrame();

	logger.fdebug("aput: valueReg: {}, arrayReg: {}, indexReg: {}", valueReg, arrayReg, indexReg);

	auto arrayObj = frame.getObjRegister(arrayReg);
	if (arrayObj->isNull()) {
		throw NullPointerException("aput on null array object");
	}
	auto array = static_cast<ArrayRef>(arrayObj);
	if (!array) {
		throw ClassCastException("aput: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	int32_t value = frame.getIntRegister(valueReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aput: Array index out of bounds");
	}
	array->setRawElement(index, static_cast<uint32_t>(value));
	frame.pc() += 3;
}
// aput-wide vAA, vBB, vCC
void Interpreter::aput_wide(const uint8_t* operand_) {
	uint8_t valueReg = operand_[0];
	uint8_t arrayReg = operand_[1];
	uint8_t indexReg = operand_[2];
	auto& frame = _rt.currentFrame();

	auto arrayObj = frame.getObjRegister(arrayReg);
	if (arrayObj->isNull()) {
		throw NullPointerException("aput-wide on null array object");
	}
	auto array = static_cast<ArrayRef>(arrayObj);
	if (!array) {
		throw ClassCastException("aput-wide: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aput-wide: Array index out of bounds");
	}
	auto value = frame.getLongRegister(valueReg);
	array->setRawElement(index, static_cast<uint64_t>(value));
	frame.pc() += 3;
}
// aput-object vAA, vBB, vCC
void Interpreter::aput_object(const uint8_t* operand_) {
	uint8_t valueReg = operand_[0];
	uint8_t arrayReg = operand_[1];
	uint8_t indexReg = operand_[2];
	auto& frame = _rt.currentFrame();

	auto arrayObj = frame.getObjRegister(arrayReg);
	if (arrayObj->isNull()) {
		throw NullPointerException("aput-object on null array object");
	}
	auto array = static_cast<ArrayRef>(arrayObj);
	if (!array) {
		throw ClassCastException("aput-object: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	auto value = frame.getObjRegister(valueReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aput-object: Array index out of bounds");
	}
	array->setElement(index, value);
	frame.pc() += 3;
}
// aput-boolean vAA, vBB, vCC
void Interpreter::aput_boolean(const uint8_t* operand_) {
	uint8_t valueReg = operand_[0];
	uint8_t arrayReg = operand_[1];
	uint8_t indexReg = operand_[2];
	auto& frame = _rt.currentFrame();

	auto arrayObj = frame.getObjRegister(arrayReg);
	if (arrayObj->isNull()) {
		throw NullPointerException("aput-boolean on null array object");
	}
	auto array = static_cast<ArrayRef>(arrayObj);
	if (!array) {
		throw ClassCastException("aput-boolean: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	bool value = static_cast<uint8_t>(frame.getIntRegister(valueReg)) != 0;
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aput-boolean: Array index out of bounds");
	}
	array->setRawElement(index, value ? 1u : 0u);
	frame.pc() += 3;
}
// aput-byte vAA, vBB, vCC
void Interpreter::aput_byte(const uint8_t* operand_) {
	uint8_t valueReg = operand_[0];
	uint8_t arrayReg = operand_[1];
	uint8_t indexReg = operand_[2];
	auto& frame = _rt.currentFrame();

	auto arrayObj = frame.getObjRegister(arrayReg);
	if (arrayObj->isNull()) {
		throw NullPointerException("aput-byte on null array object");
	}
	auto array = static_cast<ArrayRef>(arrayObj);
	if (!array) {
		throw ClassCastException("aput-byte: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	int8_t value = static_cast<int8_t>(frame.getIntRegister(valueReg));
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aput-byte: Array index out of bounds");
	}
	array->setRawElement(index, static_cast<uint32_t>(static_cast<int32_t>(value)));
	frame.pc() += 3;
}
// aput-char vAA, vBB, vCC
void Interpreter::aput_char(const uint8_t* operand_) {
	uint8_t valueReg = operand_[0];
	uint8_t arrayReg = operand_[1];
	uint8_t indexReg = operand_[2];
	auto& frame = _rt.currentFrame();

	auto arrayObj = frame.getObjRegister(arrayReg);
	if (arrayObj->isNull()) {
		throw NullPointerException("aput-char on null array object");
	}
	auto array = static_cast<ArrayRef>(arrayObj);
	if (!array) {
		throw ClassCastException("aput-char: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	uint16_t value = static_cast<uint16_t>(frame.getIntRegister(valueReg));
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aput-char: Array index out of bounds");
	}
	array->setRawElement(index, value);
	frame.pc() += 3;
}
// aput-short vAA, vBB, vCC
void Interpreter::aput_short(const uint8_t* operand_) {
	uint8_t valueReg = operand_[0];
	uint8_t arrayReg = operand_[1];
	uint8_t indexReg = operand_[2];
	auto& frame = _rt.currentFrame();

	auto arrayObj = frame.getObjRegister(arrayReg);
	if (arrayObj->isNull()) {
		throw NullPointerException("aput-short on null array object");
	}
	auto array = static_cast<ArrayRef>(arrayObj);
	if (!array) {
		throw ClassCastException("aput-short: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	int16_t value = static_cast<int16_t>(frame.getIntRegister(valueReg));
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aput-short: Array index out of bounds");
	}
	array->setRawElement(index, static_cast<uint32_t>(static_cast<int32_t>(value)));
	frame.pc() += 3;
}
// iget vA, vB, field@CCCC
void Interpreter::iget(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iget on null object");
	}

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "I" && field.getType() != "F") {
		throw VmException("iget: Field {} type mismatch, expected int but got {}", field.getName(), field.getType());
	}

	auto fieldObj = obj->getField(field.getName());
	if (!fieldObj || !fieldObj->isNumberObject()) {
		throw VmException("iget: Field {} is not a number object", field.getName());
	}
	int32_t value = fieldObj->getValue();
	logger.fdebug("iget {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// iget-wide vA, vB, field@CCCC
void Interpreter::iget_wide(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iget_wide on null object");
	}

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "J" && field.getType() != "D") {
		throw VmException("iget_wide: Field {} type mismatch, expected long or double but got {}", field.getName(), field.getType());
	}
	auto fieldObj = obj->getField(field.getName());
	if (!fieldObj || !fieldObj->isNumberObject()) {
		throw VmException("iget_wide: Field {} is not a number object", field.getName());
	}

	auto value = fieldObj->getLongValue();
	logger.fdebug("iget_wide {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	frame.setLongRegister(dest, value);
	frame.pc() += 3;
}
// iget-object vA, vB, field@CCCC
void Interpreter::iget_object(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iget_object on null object");
	}

	// class should have been loaded if trying to access an instance field. resolveField should not fail.
	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType()[0] != 'L' && field.getType()[0] != '[') {
		throw VmException("iget_object: Field {} type mismatch, expected object or array but got {}", field.getName(), field.getType());
	}

	auto fieldObj = obj->getField(field.getName());
	logger.fdebug("iget_object {}.{}={}", field.getClass().getFullname(), field.getName(), fieldObj ? fieldObj->toString() : "null");
	frame.setObjRegister(dest, fieldObj);
	frame.pc() += 3;
}
// iget-boolean vA, vB, field@CCCC
void Interpreter::iget_boolean(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iget_boolean on null object");
	}

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "Z") {
		throw VmException("iget_boolean: Field {} type mismatch, expected boolean but got {}", field.getName(), field.getType());
	}

	auto fieldObj = obj->getField(field.getName());
	if (!fieldObj || !fieldObj->isNumberObject()) {
		throw VmException("iget_boolean: Field {} is not a number object", field.getName());
	}
	bool value = static_cast<uint8_t>(fieldObj->getValue()) != 0;
	logger.fdebug("iget_boolean {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// iget-byte vA, vB, field@CCCC
void Interpreter::iget_byte(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iget_byte on null object");
	}

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "B") {
		throw VmException("iget_byte: Field {} type mismatch, expected byte but got {}", field.getName(), field.getType());
	}

	auto fieldObj = obj->getField(field.getName());
	if (!fieldObj || !fieldObj->isNumberObject()) {
		throw VmException("iget_byte: Field {} is not a number object", field.getName());
	}
	int8_t value = static_cast<int8_t>(fieldObj->getValue());
	logger.fdebug("iget_byte {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// iget-char vA, vB, field@CCCC
void Interpreter::iget_char(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iget_char on null object");
	}

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "C") {
		throw VmException("iget_char: Field {} type mismatch, expected char but got {}", field.getName(), field.getType());
	}

	auto fieldObj = obj->getField(field.getName());
	if (!fieldObj || !fieldObj->isNumberObject()) {
		throw VmException("iget_byte: Field {} is not a number object", field.getName());
	}
	uint16_t value = static_cast<uint16_t>(fieldObj->getValue());
	logger.fdebug("iget_char {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// iget-short vA, vB, field@CCCC
void Interpreter::iget_short(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iget_short on null object");
	}

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "S") {
		throw VmException("iget_short: Field {} type mismatch, expected short but got {}", field.getName(), field.getType());
	}

	auto fieldObj = obj->getField(field.getName());
	if (!fieldObj || !fieldObj->isNumberObject()) {
		throw VmException("iget_byte: Field {} is not a number object", field.getName());
	}
	int16_t value = static_cast<int16_t>(fieldObj->getValue());
	logger.fdebug("iget_short {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// iput vA, vB, field@CCCC
void Interpreter::iput(const uint8_t* operand_) {
	uint8_t src = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iput on null object");
	}

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "I" && field.getType() != "F") {
		throw VmException("iput: Field {} type mismatch, expected int but got {}", field.getName(), field.getType());
	}
	int32_t value = frame.getIntRegister(src);
	logger.fdebug("iput {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	obj->setFieldValue(field.getName(), static_cast<uint32_t>(value));
	frame.pc() += 3;
}
// iput-wide vA, vB, field@CCCC
void Interpreter::iput_wide(const uint8_t* operand_) {
	uint8_t src = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iput_wide on null object");
	}

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.isStatic()) {
		throw VmException("iput_wide: Cannot use iput_wide on a static field");
	}

	if (field.getType() != "J" && field.getType() != "D") {
		throw VmException("iput_wide: Field {} type mismatch, expected long or double but got {}", field.getName(), field.getType());
	}

	int64_t value = frame.getLongRegister(src);
	logger.fdebug("iput_wide {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	obj->setFieldValue(field.getName(), static_cast<uint64_t>(value));
	frame.pc() += 3;
}
// iput-object vA, vB, field@CCCC
void Interpreter::iput_object(const uint8_t* operand_) {
	uint8_t src = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iput_object on null object");
	}

	// class should have been loaded if trying to access an instance field. resolveField should not fail.
	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.isStatic()) {
		throw VmException("iput_object: Cannot use iput_object on a static field");
	}

	if (field.getType()[0] != 'L' && field.getType()[0] != '[') {
		throw VmException("iput_object: Field {} type mismatch, expected object or array but got {}", field.getName(), field.getType());
	}

	auto value = frame.getObjRegister(src);
	logger.fdebug("iput_object {}.{}={}", field.getClass().getFullname(), field.getName(), value->toString());
	obj->setField(field.getName(), value);
	frame.pc() += 3;
}
// iput-boolean vA, vB, field@CCCC
void Interpreter::iput_boolean(const uint8_t* operand_) {
	uint8_t src = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iput_boolean on null object");
	}

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "Z") {
		throw VmException("iput_boolean: Field {} type mismatch, expected boolean but got {}", field.getName(), field.getType());
	}

	bool value = static_cast<uint8_t>(frame.getIntRegister(src)) != 0;
	logger.fdebug("iput_boolean {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	obj->setFieldValue(field.getName(), value ? 1u : 0u);
	frame.pc() += 3;
}
// iput-byte vA, vB, field@CCCC
void Interpreter::iput_byte(const uint8_t* operand_) {
	uint8_t src = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iput_byte on null object");
	}

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "B") {
		throw VmException("iput_byte: Field {} type mismatch, expected byte but got {}", field.getName(), field.getType());
	}

	int8_t value = static_cast<int8_t>(frame.getIntRegister(src));
	logger.fdebug("iput_byte {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	obj->setFieldValue(field.getName(), static_cast<uint32_t>(static_cast<int32_t>(value)));
	frame.pc() += 3;
}
// iput-char vA, vB, field@CCCC
void Interpreter::iput_char(const uint8_t* operand_) {
	uint8_t src = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iput_char on null object");
	}

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "C") {
		throw VmException("iput_char: Field {} type mismatch, expected char but got {}", field.getName(), field.getType());
	}

	uint16_t value = static_cast<uint16_t>(frame.getIntRegister(src));
	logger.fdebug("iput_char {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	obj->setFieldValue(field.getName(), value);
	frame.pc() += 3;
}
// iput-short vA, vB, field@CCCC
void Interpreter::iput_short(const uint8_t* operand_) {
	uint8_t src = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iput_short on null object");
	}

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "S") {
		throw VmException("iput_short: Field {} type mismatch, expected short but got {}", field.getName(), field.getType());
	}

	int16_t value = static_cast<int16_t>(frame.getIntRegister(src));
	logger.fdebug("iput_short {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	obj->setFieldValue(field.getName(), static_cast<uint32_t>(static_cast<int32_t>(value)));
	frame.pc() += 3;
}
// sget vA, field@BBBB
void Interpreter::sget(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw VmException("sget: Cannot use sget on a non-static field");
	}
	if (field.getType() != "I" && field.getType() != "F") {
		throw VmException("sget: Field {} type mismatch, expected int but got {}", field.getName(), field.getType());
	}
	// static field access, class instance may not be instantiated yet
	auto& clazz = field.getClass();
	if (!clazz.isStaticInitialized()) {
		executeClinit(clazz);
	}
	int32_t value = field.getIntValue();
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// sget-wide vA, field@BBBB
void Interpreter::sget_wide(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw VmException("sget_wide: Cannot use sget on a non-static field");
	}
	if (field.getType() != "J" && field.getType() != "D") {
		throw VmException("sget_wide: Field {} type mismatch, expected long or double but got {}", field.getName(), field.getType());
	}
	// static field access, class instance may not be instantiated yet
	auto& clazz = field.getClass();
	if (!clazz.isStaticInitialized()) {
		executeClinit(clazz);
	}
	int64_t value = field.getLongValue();
	frame.setLongRegister(dest, value);
	frame.pc() += 3;
}
// sget-object vA, field@BBBB
void Interpreter::sget_object(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw VmException("sget_object: Cannot use sget_object on a non-static field");
	}
	if (field.getType()[0] != 'L' && field.getType()[0] != '[') {
		throw VmException("sget_object: Field {} type mismatch, expected object but got {}", field.getName(), field.getType());
	}
	// static field access, class instance may not be instantiated yet
	auto& clazz = field.getClass();
	if (!clazz.isStaticInitialized()) {
		executeClinit(clazz);
	}
	// set result of the sget-object to the destination register
	frame.setObjRegister(dest, field.getObjectValue());
	frame.pc() += 3;
}
// sget-boolean vA, field@BBBB
void Interpreter::sget_boolean(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw VmException("sget_boolean: Cannot use sget_boolean on a non-static field");
	}
	if (field.getType() != "Z") {
		throw VmException("sget_boolean: Field {} type mismatch, expected boolean but got {}", field.getName(), field.getType());
	}
	// static field access, class instance may not be instantiated yet
	auto& clazz = field.getClass();
	if (!clazz.isStaticInitialized()) {
		executeClinit(clazz);
	}
	bool value = static_cast<uint8_t>(field.getIntValue()) != 0;
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// sget-byte vA, field@BBBB
void Interpreter::sget_byte(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw VmException("sget_byte: Cannot use sget_byte on a non-static field");
	}
	if (field.getType() != "B") {
		throw VmException("sget_byte: Field {} type mismatch, expected byte but got {}", field.getName(), field.getType());
	}
	// static field access, class instance may not be instantiated yet
	auto& clazz = field.getClass();
	if (!clazz.isStaticInitialized()) {
		executeClinit(clazz);
	}
	int8_t value = static_cast<int8_t>(field.getIntValue());
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// sget-char vA, field@BBBB
void Interpreter::sget_char(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw VmException("sget_char: Cannot use sget on a non-static field");
	}
	if (field.getType() != "C") {
		throw VmException("sget_char: Field {} type mismatch, expected char but got {}", field.getName(), field.getType());
	}
	// static field access, class instance may not be instantiated yet
	auto& clazz = field.getClass();
	if (!clazz.isStaticInitialized()) {
		executeClinit(clazz);
	}
	uint16_t value = static_cast<uint16_t>(field.getIntValue());
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// sget-short vA, field@BBBB
void Interpreter::sget_short(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	const auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw VmException("sget_short: Cannot use sget on a non-static field");
	}
	if (field.getType() != "S") {
		throw VmException("sget_short: Field {} type mismatch, expected short but got {}", field.getName(), field.getType());
	}
	// static field access, class instance may not be instantiated yet
	auto& clazz = field.getClass();
	if (!clazz.isStaticInitialized()) {
		executeClinit(clazz);
	}
	int16_t value = static_cast<int16_t>(field.getIntValue());
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// sput vA, field@BBBB
void Interpreter::sput(const uint8_t* operand_) {
	uint8_t src = operand_[0];
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw VmException("sput: Cannot use sput on a non-static field");
	}
	if (field.getType() != "I" && field.getType() != "F") {
		throw VmException("sput: Field {} type mismatch, expected int but got {}", field.getName(), field.getType());
	}

	int32_t value = frame.getIntRegister(src);
	logger.fdebug("sput {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	field.setIntValue(value);
	frame.pc() += 3;
}
// sput-wide vA, field@BBBB
void Interpreter::sput_wide(const uint8_t* operand_) {
	uint8_t src = operand_[0];
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw VmException("sput_wide: Cannot use sput_wide on a non-static field");
	}
	if (field.getType() != "J" && field.getType() != "D") {
		throw VmException("sput_wide: Field {} type mismatch, expected long or double but got {}", field.getName(), field.getType());
	}

	int64_t value = frame.getLongRegister(src);
	logger.fdebug("sput_wide {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	field.setLongValue(value);
	frame.pc() += 3;
}
// sput-object vA, field@BBBB
void Interpreter::sput_object(const uint8_t* operand_) {
	uint8_t src = operand_[0];
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	std::string classname, fieldname;
	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex, classname, fieldname);
	if (!field.isStatic()) {
		throw VmException("sput_object: Cannot use sput_object on a non-static field");
	}
	if (field.getType()[0] != 'L' && field.getType()[0] != '[') {
		throw VmException("sput_object: Field {} type mismatch, expected object but got {}", field.getName(), field.getType());
	}
	// static field access, class instance may not be instantiated yet
	auto& clazz = field.getClass();
	if (!clazz.isStaticInitialized()) {
		executeClinit(clazz);
	}
	// set result of the sput-object
	auto value = frame.getObjRegister(src);
	field.setObjectValue(value);

	frame.pc() += 3;
}
// sput-boolean vA, field@BBBB
void Interpreter::sput_boolean(const uint8_t* operand_) {
	uint8_t src = operand_[0];
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw VmException("sput_boolean: Cannot use sput_boolean on a non-static field");
	}
	if (field.getType() != "Z") {
		throw VmException("sput_boolean: Field {} type mismatch, expected boolean but got {}", field.getName(), field.getType());
	}

	bool value = static_cast<uint8_t>(frame.getIntRegister(src)) != 0;
	logger.fdebug("sput_boolean {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	field.setIntValue(value);
	frame.pc() += 3;
}
// sput-byte vA, field@BBBB
void Interpreter::sput_byte(const uint8_t* operand_) {
	uint8_t src = operand_[0];
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw VmException("sput_byte: Cannot use sput_byte on a non-static field");
	}
	if (field.getType() != "B") {
		throw VmException("sput_byte: Field {} type mismatch, expected byte but got {}", field.getName(), field.getType());
	}

	int8_t value = static_cast<int8_t>(frame.getIntRegister(src));
	logger.fdebug("sput_byte {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	field.setIntValue(value);
	frame.pc() += 3;
}
// sput-char vA, field@BBBB
void Interpreter::sput_char(const uint8_t* operand_) {
	uint8_t src = operand_[0];
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw VmException("sput_char: Cannot use sput_char on a non-static field");
	}
	if (field.getType() != "C") {
		throw VmException("sput_char: Field {} type mismatch, expected char but got {}", field.getName(), field.getType());
	}

	uint16_t value = static_cast<uint16_t>(frame.getIntRegister(src));
	logger.fdebug("sput_char {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	field.setIntValue(value);
	frame.pc() += 3;
}
// sput-short vA, field@BBBB
void Interpreter::sput_short(const uint8_t* operand_) {
	uint8_t src = operand_[0];
	uint16_t fieldIndex = *(const uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw VmException("sput_short: Cannot use sput_short on a non-static field");
	}
	if (field.getType() != "S") {
		throw VmException("sput_short: Field {} type mismatch, expected short but got {}", field.getName(), field.getType());
	}

	int16_t value = static_cast<int16_t>(frame.getIntRegister(src));
	logger.fdebug("sput_short {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	field.setIntValue(value);
	frame.pc() += 3;
}
// invoke-virtual {vD, vE, vF, vG, vA}, meth@CCCC
void Interpreter::invoke_virtual(const uint8_t* operand_) {
	auto& classloader = _rt.getClassLoader();
	auto& frame = _rt.currentFrame();
	uint16_t methodRef = *(const uint16_t*)&operand_[1];

	auto args = getInvokeMethodArgs(operand_);
	auto this_ptr = args[0];
	if (this_ptr->isNull()) {
		const auto& caller = frame.getMethod();
		throw NullPointerException(
		    fmt::format("invoke-virtual on null object in {}.{}{}", caller.getClass().getFullname(), caller.getName(), caller.getSignature()));
	}
	if (!this_ptr->isClass()) {
		throw VmException("invoke-virtual: this pointer is not an ObjectClass, got {}", this_ptr->toString());
	}
	Class* instance = &this_ptr->getClass();
	if (!instance->isStaticInitialized()) {
		executeClinit(*instance);
	}

	std::string classname, methodname, signature;
	classloader.findMethod(frame.getDexIdx(), methodRef, classname, methodname, signature);

	Class* const receiverClass = instance;
	auto& cacheEntry = _virtualCallCache[&frame.getMethod()][frame.pc()];
	Method* vmethod = nullptr;
	if (cacheEntry.receiverClass == receiverClass) {
		// Monomorphic fast path: same receiver class as last time this call site ran, so it
		// resolves to the same method - skip the hierarchy walk below entirely.
		vmethod = cacheEntry.resolvedMethod;
		instance = cacheEntry.declaringClass;
	} else {
		while (true) {
			Method* found = instance->findMethodOrNull(methodname, signature);
			if (found != nullptr) {
				vmethod = found;
				if (vmethod->isVirtual()) {
					break;  // Method found, exit loop
				}
				// not virtual: keep searching up the superclass chain (see comment below)
			}
			logger.fdebug("invoke-virtual: method {}->{}{} not found, trying superclass", instance->getFullname(), methodname, signature);
			if (instance->hasSuperClass()) {
				// If the method is not found in the current class, try the superclass
				instance = &classloader.getOrLoad(instance->getSuperClassname());
				if (!instance->isStaticInitialized()) {
					executeClinit(*instance);
				}
			} else {
				// If no superclass, break the loop
				break;
			}
		}
		cacheEntry.receiverClass = receiverClass;
		cacheEntry.declaringClass = instance;
		cacheEntry.resolvedMethod = vmethod;
	}
	if (vmethod) {
		if (!vmethod->isVirtual()) {
			logger.ferror("invoke-virtual: method {}->{}{} is not virtual", this_ptr->getClass().getFullname(), methodname, signature);
		}
		trace.logCall("invoke-virtual", instance->getFullname(), methodname, signature, args, vmethod->isStatic());
		if (vmethod->isNative()) {
			executeNativeMethod(*vmethod, args);
		} else {
			if (vmethod->hasBytecode()) {
				auto& newframe = _rt.newFrame(*vmethod);
				// When a method is invoked, the parameters to the method are placed into the last n registers.
				for (size_t i = 0; i < args.size(); ++i) {
					uint32_t regIdx = vmethod->getNbRegisters() - args.size() + i;
					newframe.setObjRegister(regIdx, args[i]);
				}
			} else {
				vmethod->execute(frame, args);
			}
		}
	} else {
		// If no method found, throw an error
		throw VmException(fmt::format("invoke-virtual: call method {}->{}{} not found", this_ptr->getClass().getFullname(), methodname, signature));
	}

	frame.pc() += 5;
}
// invoke-super {vD, vE, vF, vG, vA}, meth@CCCC
void Interpreter::invoke_super(const uint8_t* operand_) {
	auto& classloader = _rt.getClassLoader();
	auto& frame = _rt.currentFrame();
	uint16_t methodRef = *(const uint16_t*)&operand_[1];

	std::string classname, methodname, signature;
	classloader.findMethod(frame.getDexIdx(), methodRef, classname, methodname, signature);

	auto instance = &classloader.getOrLoad(classname);
	Method* vmethod = nullptr;
	while (true) {
		Method* found = instance->findMethodOrNull(methodname, signature);
		if (found != nullptr) {
			vmethod = found;
			if (vmethod->isVirtual()) {
				break;  // Method found, exit loop
			}
			// not virtual: keep searching up the superclass chain (see comment below)
		}
		logger.fdebug("invoke-supper: method {}->{}{} not found, trying superclass", classname, methodname, signature);
		if (instance->hasSuperClass()) {
			// If the method is not found in the current class, try the superclass
			instance = &classloader.getOrLoad(instance->getSuperClassname());
			if (!instance->isStaticInitialized()) {
				executeClinit(*instance);
			}
		} else {
			// If no superclass, break the loop
			break;
		}
	}

	auto args = getInvokeMethodArgs(operand_);
	trace.logCall("invoke-super", instance->getFullname(), methodname, signature, args, vmethod->isStatic());
	if (vmethod->isNative()) {
		executeNativeMethod(*vmethod, args);
	} else {
		if (!vmethod->hasBytecode()) {
			vmethod->execute(frame, args);
		} else {
			auto& newframe = _rt.newFrame(*vmethod);
			// set args on new frame
			// When a method is invoked, the parameters to the method are placed into the last n registers.
			for (size_t i = 0; i < args.size(); ++i) {
				uint32_t regIdx = vmethod->getNbRegisters() - args.size() + i;
				newframe.setObjRegister(regIdx, args[i]);
			}
		}
	}
	frame.pc() += 5;
}
// invoke-direct {vD, vE, vF, vG, vA}, meth@CCCC
void Interpreter::invoke_direct(const uint8_t* operand_) {
	auto& classloader = _rt.getClassLoader();
	auto& frame = _rt.currentFrame();
	uint16_t methodRef = *(const uint16_t*)&operand_[1];
	auto& method = classloader.resolveMethod(frame.getDexIdx(), methodRef);
	auto& cls = method.getClass();
	if (!cls.isStaticInitialized()) {
		executeClinit(cls);
	}

	auto args = getInvokeMethodArgs(operand_);
	if (method.isStatic()) {
		trace.logCall("invoke-static", method.getClass().getFullname(), method.getName(), method.getSignature(), args, method.isStatic());
	} else {
		trace.logCall("invoke-direct", method.getClass().getFullname(), method.getName(), method.getSignature(), args, method.isStatic());
	}

	uint32_t thisRegIdx = operand_[3] & 0x0F;
	if (tryRedirectToStringFactory(method, args, thisRegIdx)) {
		frame.pc() += 5;
		return;
	}

	if (method.isNative()) {
		executeNativeMethod(method, args);
	} else {
		if (!method.hasBytecode()) {
			method.execute(frame, args);
		} else {
			auto& newframe = _rt.newFrame(method);
			// set args on new frame
			// When a method is invoked, the parameters to the method are placed into the last n registers.
			for (size_t i = 0; i < args.size(); ++i) {
				uint32_t regIdx = method.getNbRegisters() - args.size() + i;
				newframe.setObjRegister(regIdx, args[i]);
			}
		}
	}
	frame.pc() += 5;
}
// invoke-static {vD, vE, vF, vG, vA}, meth@CCCC
void Interpreter::invoke_static(const uint8_t* operand_) {
	invoke_direct(operand_);
}
// invoke-interface {vD, vE, vF, vG, vA}, meth@CCCC
void Interpreter::invoke_interface(const uint8_t* operand_) {
	auto& classloader = _rt.getClassLoader();
	auto& frame = _rt.currentFrame();

	uint16_t methodRef = *(const uint16_t*)&operand_[1];
	auto args = getInvokeMethodArgs(operand_);
	auto this_ptr = args[0];
	if (this_ptr->isNull()) {
		throw NullPointerException("invoke_interface on null object");
	}
	if (!this_ptr->isClass()) {
		throw VmException("invoke-interface: this pointer is not an ObjectClass, got {}", this_ptr->toString());
	}
	Class* instance = &this_ptr->getClass();
	if (!instance->isStaticInitialized()) {
		executeClinit(*instance);
	}

	std::string ifclassname, methodname, signature;
	classloader.findMethod(frame.getDexIdx(), methodRef, ifclassname, methodname, signature);

	logger.fdebug("invoke-interface {}->{}{} for class {}", ifclassname, methodname, signature, instance->getFullname());

	Method* vmethod = resolveInterfaceMethod(*instance, ifclassname, methodname, signature);
	if (vmethod) {
		if (vmethod->isStatic()) {
			throw VmException("invoke-interface: method {}->{}{} is static", ifclassname, methodname, signature);
		}
		if (!vmethod->isVirtual()) {
			logger.ferror("invoke-interface: {}->{}{} not virtual", ifclassname, methodname, signature);
		}
		trace.logCall("invoke-interface", ifclassname, methodname, signature, args, vmethod->isStatic());
		if (vmethod->isNative()) {
			executeNativeMethod(*vmethod, args);
		} else {
			auto& newframe = _rt.newFrame(*vmethod);
			// When a method is invoked, the parameters to the method are placed into the last n registers.
			for (size_t i = 0; i < args.size(); ++i) {
				uint32_t regIdx = vmethod->getNbRegisters() - args.size() + i;
				newframe.setObjRegister(regIdx, args[i]);
			}
		}
	} else {
		// If no method found, throw an error
		throw VmException("invoke-interface: call method {}->{}{} not found for instance {}", ifclassname, methodname, signature, instance->getFullname());
	}
	frame.pc() += 5;
}
// invoke-virtual/range {vCCCC .. vNNNN}, meth@BBBB
void Interpreter::invoke_virtual_range(const uint8_t* operand_) {
	const uint16_t methodRef = *reinterpret_cast<const uint16_t*>(&operand_[1]);
	const uint16_t startReg = *reinterpret_cast<const uint16_t*>(&operand_[3]);
	const uint8_t regCount = operand_[0];

	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	std::vector<ObjectRef> args;
	for (uint8_t i = 0; i < regCount; ++i) {
		args.push_back(frame.getObjRegister(startReg + i));
	}

	auto this_ptr = args[0];
	if (this_ptr->isNull()) {
		const auto& caller = frame.getMethod();
		throw NullPointerException(
		    fmt::format("invoke-virtual/range on null object in {}.{}{}", caller.getClass().getFullname(), caller.getName(), caller.getSignature()));
	}
	if (!this_ptr->isClass()) {
		throw VmException("invoke-virtual/range: this pointer is not an ObjectClass, got {}", this_ptr->toString());
	}
	Class* instance = &this_ptr->getClass();
	if (!instance->isStaticInitialized()) {
		executeClinit(*instance);
	}

	std::string classname, methodname, signature;
	classloader.findMethod(frame.getDexIdx(), methodRef, classname, methodname, signature);

	Class* const receiverClass = instance;
	auto& cacheEntry = _virtualCallCache[&frame.getMethod()][frame.pc()];
	Method* vmethod = nullptr;
	if (cacheEntry.receiverClass == receiverClass) {
		// Monomorphic fast path: same receiver class as last time this call site ran, so it
		// resolves to the same method - skip the hierarchy walk below entirely.
		vmethod = cacheEntry.resolvedMethod;
		instance = cacheEntry.declaringClass;
	} else {
		while (true) {
			Method* found = instance->findMethodOrNull(methodname, signature);
			if (found != nullptr) {
				vmethod = found;
				if (vmethod->isVirtual()) {
					break;  // Method found, exit loop
				}
				// not virtual: keep searching up the superclass chain (see comment below)
			}
			logger.fdebug("invoke-virtual/range: method {}->{}{} not found, trying superclass", instance->getFullname(), methodname, signature);
			if (instance->hasSuperClass()) {
				// If the method is not found in the current class, try the superclass
				instance = &classloader.getOrLoad(instance->getSuperClassname());
				if (!instance->isStaticInitialized()) {
					executeClinit(*instance);
				}
			} else {
				// If no superclass, break the loop
				break;
			}
		}
		cacheEntry.receiverClass = receiverClass;
		cacheEntry.declaringClass = instance;
		cacheEntry.resolvedMethod = vmethod;
	}
	if (vmethod) {
		if (vmethod->isStatic()) {
			throw VmException("invoke-virtual/range: method {}->{}{} is static", this_ptr->getClass().getFullname(), methodname, signature);
		}
		if (!vmethod->isVirtual()) {
			logger.ferror("invoke-virtual/range: method {}->{}{} is not virtual", this_ptr->getClass().getFullname(), methodname, signature);
		}
		trace.logCall("invoke-virtual/range", instance->getFullname(), methodname, signature, args, vmethod->isStatic());
		if (vmethod->isNative()) {
			executeNativeMethod(*vmethod, args);
		} else {
			if (vmethod->hasBytecode()) {
				auto& newframe = _rt.newFrame(*vmethod);
				// When a method is invoked, the parameters to the method are placed into the last n registers.
				for (size_t i = 0; i < args.size(); ++i) {
					uint32_t regIdx = vmethod->getNbRegisters() - args.size() + i;
					newframe.setObjRegister(regIdx, args[i]);
				}
			} else {
				vmethod->execute(frame, args);
			}
		}
	} else {
		// If no method found, throw an error
		throw VmException(fmt::format("invoke-virtual/range: call method {}->{}{} not found", this_ptr->getClass().getFullname(), methodname, signature));
	}

	frame.pc() += 5;
}
// invoke-super/range {vCCCC .. vNNNN}, meth@BBBB
void Interpreter::invoke_super_range(const uint8_t* operand_) {
	invoke_direct_range(operand_);
}
// invoke-direct/range {vCCCC .. vNNNN}, meth@BBBB
void Interpreter::invoke_direct_range(const uint8_t* operand_) {
	const uint16_t methodRef = *reinterpret_cast<const uint16_t*>(&operand_[1]);
	const uint16_t startReg = *reinterpret_cast<const uint16_t*>(&operand_[3]);
	const uint8_t regCount = operand_[0];

	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& method = classloader.resolveMethod(frame.getDexIdx(), methodRef);
	auto& cls = method.getClass();

	if (!cls.isStaticInitialized()) {
		executeClinit(cls);
	}

	std::vector<ObjectRef> args;
	for (uint8_t i = 0; i < regCount; ++i) {
		args.push_back(frame.getObjRegister(startReg + i));
	}

	if (method.isStatic()) {
		trace.logCall("invoke-static/range", cls.getFullname(), method.getName(), method.getSignature(), args, method.isStatic());
	} else {
		trace.logCall("invoke-direct/range", cls.getFullname(), method.getName(), method.getSignature(), args, method.isStatic());
	}
	if (method.isNative()) {
		executeNativeMethod(method, args);
	} else {
		if (!method.hasBytecode()) {
			method.execute(frame, args);
		} else {
			auto& newframe = _rt.newFrame(method);
			for (uint8_t i = 0; i < regCount; ++i) {
				newframe.setObjRegister(method.getNbRegisters() - regCount + i, frame.getObjRegister(startReg + i));
			}
		}
	}

	frame.pc() += 5;
}
// invoke-static/range {vCCCC .. vNNNN}, meth@BBBB
void Interpreter::invoke_static_range(const uint8_t* operand_) {
	invoke_direct_range(operand_);
}
// invoke-interface/range {vCCCC .. vNNNN}, meth@BBBB
void Interpreter::invoke_interface_range(const uint8_t* operand_) {
	const uint16_t methodRef = *reinterpret_cast<const uint16_t*>(&operand_[1]);
	const uint16_t startReg = *reinterpret_cast<const uint16_t*>(&operand_[3]);
	const uint8_t regCount = operand_[0];

	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	std::vector<ObjectRef> args;
	for (uint8_t i = 0; i < regCount; ++i) {
		args.push_back(frame.getObjRegister(startReg + i));
	}

	auto this_ptr = args[0];
	if (this_ptr->isNull()) {
		throw NullPointerException("invoke-virtual/range on null object");
	}
	if (!this_ptr->isClass()) {
		throw VmException("invoke-virtual/range: this pointer is not an ObjectClass, got {}", this_ptr->toString());
	}
	Class* instance = &this_ptr->getClass();
	if (!instance->isStaticInitialized()) {
		executeClinit(*instance);
	}

	std::string ifclassname, methodname, signature;
	classloader.findMethod(frame.getDexIdx(), methodRef, ifclassname, methodname, signature);

	Method* vmethod = resolveInterfaceMethod(*instance, ifclassname, methodname, signature);
	if (vmethod) {
		if (vmethod->isStatic()) {
			throw VmException("invoke-interface/range: method {}->{}{} is static", ifclassname, methodname, signature);
		}
		if (!vmethod->isVirtual()) {
			logger.ferror("invoke-interface/range: {}->{}{} not virtual", ifclassname, methodname, signature);
		}
		trace.logCall("invoke-interface/range", ifclassname, methodname, signature, args, vmethod->isStatic());
		if (vmethod->isNative()) {
			executeNativeMethod(*vmethod, args);
		} else {
			auto& newframe = _rt.newFrame(*vmethod);
			// When a method is invoked, the parameters to the method are placed into the last n registers.
			for (size_t i = 0; i < args.size(); ++i) {
				uint32_t regIdx = vmethod->getNbRegisters() - args.size() + i;
				newframe.setObjRegister(regIdx, args[i]);
			}
		}
	} else {
		// If no method found, throw an error
		throw VmException("invoke-interface/range: call method {}->{}{} not found for instance {}", ifclassname, methodname, signature,
		                  instance->getFullname());
	}

	frame.pc() += 5;
}
// neg-int vA, vB
void Interpreter::neg_int(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int32_t value = frame.getIntRegister(src);
	frame.setIntRegister(dest, -value);
	frame.pc()++;
}
// not-int vA, vB
void Interpreter::not_int(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	uint32_t value = frame.getIntRegister(src);
	frame.setIntRegister(dest, ~value);
	frame.pc()++;
}
// neg-long vA, vB
void Interpreter::neg_long(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int64_t value = frame.getLongRegister(src);
	frame.setLongRegister(dest, -value);
	frame.pc()++;
}
// not-long vA, vB
void Interpreter::not_long(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	uint64_t value = frame.getLongRegister(src);
	frame.setLongRegister(dest, ~value);
	frame.pc()++;
}
// neg-float vA, vB
void Interpreter::neg_float(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	float value = frame.getFloatRegister(src);
	frame.setFloatRegister(dest, -value);
	frame.pc()++;
}
// neg-double vA, vB
void Interpreter::neg_double(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	double value = frame.getDoubleRegister(src);
	frame.setDoubleRegister(dest, -value);
	frame.pc()++;
}
// int-to-long vA, vB
void Interpreter::int_to_long(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int32_t value = frame.getIntRegister(src);
	frame.setLongRegister(dest, static_cast<int64_t>(value));
	frame.pc()++;
}
// int-to-float vA, vB
void Interpreter::int_to_float(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int32_t value = frame.getIntRegister(src);
	frame.setFloatRegister(dest, static_cast<float>(value));
	frame.pc()++;
}
// int-to-double vA, vB
void Interpreter::int_to_double(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int32_t value = frame.getIntRegister(src);
	frame.setDoubleRegister(dest, static_cast<double>(value));
	frame.pc()++;
}
// long-to-int vA, vB
void Interpreter::long_to_int(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int64_t value = frame.getLongRegister(src);
	frame.setIntRegister(dest, static_cast<int32_t>(value));
	frame.pc()++;
}
// long-to-float vA, vB
void Interpreter::long_to_float(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int64_t value = frame.getLongRegister(src);
	frame.setFloatRegister(dest, static_cast<float>(value));
	frame.pc()++;
}
// long-to-double vA, vB
void Interpreter::long_to_double(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int64_t value = frame.getLongRegister(src);
	frame.setDoubleRegister(dest, static_cast<double>(value));
	frame.pc()++;
}
// float-to-int vA, vB
void Interpreter::float_to_int(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	float value = frame.getFloatRegister(src);
	frame.setIntRegister(dest, static_cast<int32_t>(value));
	frame.pc()++;
}
// float-to-long vA, vB
void Interpreter::float_to_long(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	float value = frame.getFloatRegister(src);
	frame.setLongRegister(dest, static_cast<int64_t>(value));
	frame.pc()++;
}
// float-to-double vA, vB
void Interpreter::float_to_double(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	float value = frame.getFloatRegister(src);
	frame.setDoubleRegister(dest, static_cast<double>(value));
	frame.pc()++;
}
// double-to-int vA, vB
void Interpreter::double_to_int(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	double value = frame.getDoubleRegister(src);
	frame.setIntRegister(dest, static_cast<int32_t>(value));
	frame.pc()++;
}
// double-to-long vA, vB
void Interpreter::double_to_long(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	double value = frame.getDoubleRegister(src);
	frame.setLongRegister(dest, static_cast<int64_t>(value));
	frame.pc()++;
}
// double-to-float vA, vB
void Interpreter::double_to_float(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	double value = frame.getDoubleRegister(src);
	frame.setFloatRegister(dest, static_cast<float>(value));
	frame.pc()++;
}
// int-to-byte
void Interpreter::int_to_byte(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int32_t value = frame.getIntRegister(src);
	frame.setIntRegister(dest, static_cast<int8_t>(value));
	frame.pc()++;
}
// int-to-char
void Interpreter::int_to_char(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int32_t value = frame.getIntRegister(src);
	frame.setIntRegister(dest, static_cast<uint16_t>(value));
	frame.pc()++;
}
// int-to-short
void Interpreter::int_to_short(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int32_t value = frame.getIntRegister(src);
	frame.setIntRegister(dest, static_cast<int16_t>(value));
	frame.pc()++;
}
// add-int vAA, vBB, vCC
void Interpreter::add_int(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src1) + frame.getIntRegister(src2);
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// sub-int vAA, vBB, vCC
void Interpreter::sub_int(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src1) - frame.getIntRegister(src2);
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// mul-int vAA, vBB, vCC
void Interpreter::mul_int(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src1) * frame.getIntRegister(src2);
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// div-int vAA, vBB, vCC
void Interpreter::div_int(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int32_t divisor = frame.getIntRegister(src2);
	if (divisor == 0) {
		throw ArithmeticException("Division by zero in div-int");
	}
	int32_t result = frame.getIntRegister(src1) / divisor;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// rem-int vAA, vBB, vCC
void Interpreter::rem_int(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int32_t divisor = frame.getIntRegister(src2);
	if (divisor == 0) {
		throw ArithmeticException("Division by zero in rem-int");
	}
	int32_t result = frame.getIntRegister(src1) % divisor;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// and-int vAA, vBB, vCC
void Interpreter::and_int(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src1) & frame.getIntRegister(src2);
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// or-int vAA, vBB, vCC
void Interpreter::or_int(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src1) | frame.getIntRegister(src2);
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// xor-int vAA, vBB, vCC
void Interpreter::xor_int(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src1) ^ frame.getIntRegister(src2);
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// shl-int vAA, vBB, vCC
void Interpreter::shl_int(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src1) << frame.getIntRegister(src2);
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// shr-int vAA, vBB, vCC
void Interpreter::shr_int(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src1) >> frame.getIntRegister(src2);
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// ushr-int vAA, vBB, vCC
void Interpreter::ushr_int(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	uint32_t value = static_cast<uint32_t>(frame.getIntRegister(src1));
	uint32_t result = value >> frame.getIntRegister(src2);
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// add-long vAA, vBB, vCC
void Interpreter::add_long(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int64_t result = frame.getLongRegister(src1) + frame.getLongRegister(src2);
	frame.setLongRegister(dest, result);
	frame.pc() += 3;
}
// sub-long vAA, vBB, vCC
void Interpreter::sub_long(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int64_t result = frame.getLongRegister(src1) - frame.getLongRegister(src2);
	frame.setLongRegister(dest, result);
	frame.pc() += 3;
}
// mul-long vAA, vBB, vCC
void Interpreter::mul_long(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int64_t result = frame.getLongRegister(src1) * frame.getLongRegister(src2);
	frame.setLongRegister(dest, result);
	frame.pc() += 3;
}
// div-long vAA, vBB, vCC
void Interpreter::div_long(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int64_t divisor = frame.getLongRegister(src2);
	if (divisor == 0) {
		throw ArithmeticException("Division by zero in div-long");
	}
	int64_t result = frame.getLongRegister(src1) / divisor;
	frame.setLongRegister(dest, result);
	frame.pc() += 3;
}
// rem-long vAA, vBB, vCC
void Interpreter::rem_long(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int64_t divisor = frame.getLongRegister(src2);
	if (divisor == 0) {
		throw ArithmeticException("Division by zero in rem-long");
	}
	int64_t result = frame.getLongRegister(src1) % divisor;
	frame.setLongRegister(dest, result);
	frame.pc() += 3;
}
// and-long vAA, vBB, vCC
void Interpreter::and_long(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int64_t result = frame.getLongRegister(src1) & frame.getLongRegister(src2);
	frame.setLongRegister(dest, result);
	frame.pc() += 3;
}
// or-long vAA, vBB, vCC
void Interpreter::or_long(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int64_t result = frame.getLongRegister(src1) | frame.getLongRegister(src2);
	frame.setLongRegister(dest, result);
	frame.pc() += 3;
}
// xor-long vAA, vBB, vCC
void Interpreter::xor_long(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int64_t result = frame.getLongRegister(src1) ^ frame.getLongRegister(src2);
	frame.setLongRegister(dest, result);
	frame.pc() += 3;
}
// shl-long vAA, vBB, vCC
void Interpreter::shl_long(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int64_t result = frame.getLongRegister(src1) << frame.getIntRegister(src2);
	frame.setLongRegister(dest, result);
	frame.pc() += 3;
}
// shr-long vAA, vBB, vCC
void Interpreter::shr_long(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	int64_t result = frame.getLongRegister(src1) >> frame.getIntRegister(src2);
	frame.setLongRegister(dest, result);
	frame.pc() += 3;
}
// ushr-long vAA, vBB, vCC
void Interpreter::ushr_long(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	uint64_t value = static_cast<uint64_t>(frame.getLongRegister(src1));
	uint64_t result = value >> frame.getIntRegister(src2);
	frame.setLongRegister(dest, result);
	frame.pc() += 3;
}
// add-float vAA, vBB, vCC
void Interpreter::add_float(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	float result = frame.getFloatRegister(src1) + frame.getFloatRegister(src2);
	frame.setFloatRegister(dest, result);
	frame.pc() += 3;
}
// sub-float vAA, vBB, vCC
void Interpreter::sub_float(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	float result = frame.getFloatRegister(src1) - frame.getFloatRegister(src2);
	frame.setFloatRegister(dest, result);
	frame.pc() += 3;
}
// mul-float vAA, vBB, vCC
void Interpreter::mul_float(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	float result = frame.getFloatRegister(src1) * frame.getFloatRegister(src2);
	frame.setFloatRegister(dest, result);
	frame.pc() += 3;
}
// div-float vAA, vBB, vCC
void Interpreter::div_float(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	float divisor = frame.getFloatRegister(src2);
	if (divisor == 0.0f) {
		throw ArithmeticException("Division by zero in div-float");
	}
	float result = frame.getFloatRegister(src1) / divisor;
	frame.setFloatRegister(dest, result);
	frame.pc() += 3;
}
// rem-float vAA, vBB, vCC
void Interpreter::rem_float(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	float divisor = frame.getFloatRegister(src2);
	if (divisor == 0.0f) {
		throw ArithmeticException("Division by zero in rem-float");
	}
	float result = std::fmod(frame.getFloatRegister(src1), divisor);
	frame.setFloatRegister(dest, result);
	frame.pc() += 3;
}
// add-double vAA, vBB, vCC
void Interpreter::add_double(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	double result = frame.getDoubleRegister(src1) + frame.getDoubleRegister(src2);
	frame.setDoubleRegister(dest, result);
	frame.pc() += 3;
}
// sub-double vAA, vBB, vCC
void Interpreter::sub_double(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	double result = frame.getDoubleRegister(src1) - frame.getDoubleRegister(src2);
	frame.setDoubleRegister(dest, result);
	frame.pc() += 3;
}
// mul-double vAA, vBB, vCC
void Interpreter::mul_double(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	double result = frame.getDoubleRegister(src1) * frame.getDoubleRegister(src2);
	frame.setDoubleRegister(dest, result);
	frame.pc() += 3;
}
// div-double vAA, vBB, vCC
void Interpreter::div_double(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	double divisor = frame.getDoubleRegister(src2);
	if (divisor == 0.0) {
		throw ArithmeticException("Division by zero in div-double");
	}
	double result = frame.getDoubleRegister(src1) / divisor;
	frame.setDoubleRegister(dest, result);
	frame.pc() += 3;
}
// rem-double vAA, vBB, vCC
void Interpreter::rem_double(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src1 = operand_[1];
	uint8_t src2 = operand_[2];
	auto& frame = _rt.currentFrame();
	double divisor = frame.getDoubleRegister(src2);
	if (divisor == 0.0) {
		throw ArithmeticException("Division by zero in rem-double");
	}
	double result = std::fmod(frame.getDoubleRegister(src1), divisor);
	frame.setDoubleRegister(dest, result);
	frame.pc() += 3;
}
// add-int/2addr vA, vB
void Interpreter::add_int_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(dest) + frame.getIntRegister(src);
	frame.setIntRegister(dest, result);
	frame.pc()++;
}
// sub-int/2addr vA, vB
void Interpreter::sub_int_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(dest) - frame.getIntRegister(src);
	frame.setIntRegister(dest, result);
	frame.pc()++;
}
// mul-int/2addr vA, vB
void Interpreter::mul_int_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(dest) * frame.getIntRegister(src);
	frame.setIntRegister(dest, result);
	frame.pc()++;
}
// div-int/2addr vA, vB
void Interpreter::div_int_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int32_t divisor = frame.getIntRegister(src);
	if (divisor == 0) {
		throw ArithmeticException("Division by zero in div-int/2addr");
	}
	int32_t result = frame.getIntRegister(dest) / divisor;
	frame.setIntRegister(dest, result);
	frame.pc()++;
}
// rem-int/2addr vA, vB
void Interpreter::rem_int_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int32_t divisor = frame.getIntRegister(src);
	if (divisor == 0) {
		throw ArithmeticException("Division by zero in rem-int/2addr");
	}
	int32_t result = frame.getIntRegister(dest) % divisor;
	frame.setIntRegister(dest, result);
	frame.pc()++;
}
// and-int/2addr vA, vB
void Interpreter::and_int_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(dest) & frame.getIntRegister(src);
	frame.setIntRegister(dest, result);
	frame.pc()++;
}
// or-int/2addr vA, vB
void Interpreter::or_int_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(dest) | frame.getIntRegister(src);
	frame.setIntRegister(dest, result);
	frame.pc()++;
}
// xor-int/2addr vA, vB
void Interpreter::xor_int_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(dest) ^ frame.getIntRegister(src);
	frame.setIntRegister(dest, result);
	frame.pc()++;
}
// shl-int/2addr vA, vB
void Interpreter::shl_int_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(dest) << frame.getIntRegister(src);
	frame.setIntRegister(dest, result);
	frame.pc()++;
}
// shr-int/2addr vA, vB
void Interpreter::shr_int_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(dest) >> frame.getIntRegister(src);
	frame.setIntRegister(dest, result);
	frame.pc()++;
}
// ushr-int/2addr vA, vB
void Interpreter::ushr_int_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	uint32_t value = static_cast<uint32_t>(frame.getIntRegister(dest));
	uint32_t result = value >> frame.getIntRegister(src);
	frame.setIntRegister(dest, result);
	frame.pc()++;
}
// add-long/2addr vA, vB
void Interpreter::add_long_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int64_t result = frame.getLongRegister(dest) + frame.getLongRegister(src);
	frame.setLongRegister(dest, result);
	frame.pc()++;
}
// sub-long/2addr vA, vB
void Interpreter::sub_long_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int64_t result = frame.getLongRegister(dest) - frame.getLongRegister(src);
	frame.setLongRegister(dest, result);
	frame.pc()++;
}
// mul-long/2addr vA, vB
void Interpreter::mul_long_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int64_t result = frame.getLongRegister(dest) * frame.getLongRegister(src);
	frame.setLongRegister(dest, result);
	frame.pc()++;
}
// div-long/2addr vA, vB
void Interpreter::div_long_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int64_t divisor = frame.getLongRegister(src);
	if (divisor == 0) {
		throw ArithmeticException("Division by zero in div-long/2addr");
	}
	int64_t result = frame.getLongRegister(dest) / divisor;
	frame.setLongRegister(dest, result);
	frame.pc()++;
}
// rem-long/2addr vA, vB
void Interpreter::rem_long_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int64_t divisor = frame.getLongRegister(src);
	if (divisor == 0) {
		throw ArithmeticException("Division by zero in rem-long/2addr");
	}
	int64_t result = frame.getLongRegister(dest) % divisor;
	frame.setLongRegister(dest, result);
	frame.pc()++;
}
// and-long/2addr vA, vB
void Interpreter::and_long_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int64_t result = frame.getLongRegister(dest) & frame.getLongRegister(src);
	frame.setLongRegister(dest, result);
	frame.pc()++;
}
// or-long/2addr vA, vB
void Interpreter::or_long_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int64_t result = frame.getLongRegister(dest) | frame.getLongRegister(src);
	frame.setLongRegister(dest, result);
	frame.pc()++;
}
// xor-long/2addr vA, vB
void Interpreter::xor_long_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int64_t result = frame.getLongRegister(dest) ^ frame.getLongRegister(src);
	frame.setLongRegister(dest, result);
	frame.pc()++;
}
// shl-long/2addr vA, vB
void Interpreter::shl_long_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int64_t result = frame.getLongRegister(dest) << frame.getIntRegister(src);
	frame.setLongRegister(dest, result);
	frame.pc()++;
}
// shr-long/2addr vA, vB
void Interpreter::shr_long_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	int64_t result = frame.getLongRegister(dest) >> frame.getIntRegister(src);
	frame.setLongRegister(dest, result);
	frame.pc()++;
}
// ushr-long/2addr vA, vB
void Interpreter::ushr_long_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	uint64_t value = static_cast<uint64_t>(frame.getLongRegister(dest));
	uint64_t result = value >> frame.getIntRegister(src);
	frame.setLongRegister(dest, result);
	frame.pc()++;
}
// add-float/2addr vA, vB
void Interpreter::add_float_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	float result = frame.getFloatRegister(dest) + frame.getFloatRegister(src);
	frame.setFloatRegister(dest, result);
	frame.pc()++;
}
// sub-float/2addr vA, vB
void Interpreter::sub_float_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	float result = frame.getFloatRegister(dest) - frame.getFloatRegister(src);
	frame.setFloatRegister(dest, result);
	frame.pc()++;
}
// mul-float/2addr vA, vB
void Interpreter::mul_float_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	float result = frame.getFloatRegister(dest) * frame.getFloatRegister(src);
	frame.setFloatRegister(dest, result);
	frame.pc()++;
}
// div-float/2addr vA, vB
void Interpreter::div_float_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	float divisor = frame.getFloatRegister(src);
	if (divisor == 0.0f) {
		throw ArithmeticException("Division by zero in div-float/2addr");
	}
	float result = frame.getFloatRegister(dest) / divisor;
	frame.setFloatRegister(dest, result);
	frame.pc()++;
}
// rem-float/2addr vA, vB
void Interpreter::rem_float_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	float divisor = frame.getFloatRegister(src);
	if (divisor == 0.0f) {
		throw ArithmeticException("Division by zero in rem-float/2addr");
	}
	float result = std::fmod(frame.getFloatRegister(dest), divisor);
	frame.setFloatRegister(dest, result);
	frame.pc()++;
}
// add-double/2addr vA, vB
void Interpreter::add_double_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	double result = frame.getDoubleRegister(dest) + frame.getDoubleRegister(src);
	frame.setDoubleRegister(dest, result);
	frame.pc()++;
}
// sub-double/2addr vA, vB
void Interpreter::sub_double_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	double result = frame.getDoubleRegister(dest) - frame.getDoubleRegister(src);
	frame.setDoubleRegister(dest, result);
	frame.pc()++;
}
// mul-double/2addr vA, vB
void Interpreter::mul_double_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	double result = frame.getDoubleRegister(dest) * frame.getDoubleRegister(src);
	frame.setDoubleRegister(dest, result);
	frame.pc()++;
}
// div-double/2addr vA, vB
void Interpreter::div_double_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	double divisor = frame.getDoubleRegister(src);
	if (divisor == 0.0) {
		throw ArithmeticException("Division by zero in div-double/2addr");
	}
	double result = frame.getDoubleRegister(dest) / divisor;
	frame.setDoubleRegister(dest, result);
	frame.pc()++;
}
// rem-double/2addr vA, vB
void Interpreter::rem_double_2addr(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	auto& frame = _rt.currentFrame();
	double divisor = frame.getDoubleRegister(src);
	if (divisor == 0.0) {
		throw ArithmeticException("Division by zero in rem-double/2addr");
	}
	double result = std::fmod(frame.getDoubleRegister(dest), divisor);
	frame.setDoubleRegister(dest, result);
	frame.pc()++;
}
// add-int/lit16 vA, vB, #+CCCC
void Interpreter::add_int_lit16(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	int16_t literal = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src) + literal;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// rsub-int/lit16 vA, vB, #+CCCC
void Interpreter::rsub_int_lit16(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	int16_t literal = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	int32_t result = literal - frame.getIntRegister(src);
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// mul-int/lit16 vA, vB, #+CCCC
void Interpreter::mul_int_lit16(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	int16_t literal = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src) * literal;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// div-int/lit16 vA, vB, #+CCCC
void Interpreter::div_int_lit16(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	int16_t literal = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	if (literal == 0) {
		throw ArithmeticException("Division by zero in div-int/lit16");
	}
	int32_t result = frame.getIntRegister(src) / literal;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// rem-int/lit16 vA, vB, #+CCCC
void Interpreter::rem_int_lit16(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	int16_t literal = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	if (literal == 0) {
		throw ArithmeticException("Division by zero in rem-int/lit16");
	}
	int32_t result = frame.getIntRegister(src) % literal;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// and-int/lit16 vA, vB, #+CCCC
void Interpreter::and_int_lit16(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	int16_t literal = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src) & literal;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// or-int/lit16 vA, vB, #+CCCC
void Interpreter::or_int_lit16(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	int16_t literal = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src) | literal;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// xor-int/lit16 vA, vB, #+CCCC
void Interpreter::xor_int_lit16(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	int16_t literal = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src) ^ literal;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// add-int/lit8 vAA, vBB, #+CC
void Interpreter::add_int_lit8(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src = operand_[1];
	int8_t literal = static_cast<int8_t>(operand_[2]);
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src) + literal;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// rsub-int/lit8 vAA, vBB, #+CC
void Interpreter::rsub_int_lit8(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src = operand_[1];
	int8_t literal = static_cast<int8_t>(operand_[2]);
	auto& frame = _rt.currentFrame();
	int32_t result = literal - frame.getIntRegister(src);
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// mul-int/lit8 vAA, vBB, #+CC
void Interpreter::mul_int_lit8(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src = operand_[1];
	int8_t literal = static_cast<int8_t>(operand_[2]);
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src) * literal;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// div-int/lit8 vAA, vBB, #+CC
void Interpreter::div_int_lit8(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src = operand_[1];
	int8_t literal = static_cast<int8_t>(operand_[2]);
	auto& frame = _rt.currentFrame();
	if (literal == 0) {
		throw ArithmeticException("Division by zero in div-int/lit8");
	}
	int32_t result = frame.getIntRegister(src) / literal;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// rem-int/lit8 vAA, vBB, #+CC
void Interpreter::rem_int_lit8(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src = operand_[1];
	int8_t literal = static_cast<int8_t>(operand_[2]);
	auto& frame = _rt.currentFrame();
	if (literal == 0) {
		throw ArithmeticException("Division by zero in rem-int/lit8");
	}
	int32_t result = frame.getIntRegister(src) % literal;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// and-int/lit8 vAA, vBB, #+CC
void Interpreter::and_int_lit8(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src = operand_[1];
	int8_t literal = static_cast<int8_t>(operand_[2]);
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src) & literal;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// or-int/lit8 vAA, vBB, #+CC
void Interpreter::or_int_lit8(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src = operand_[1];
	int8_t literal = static_cast<int8_t>(operand_[2]);
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src) | literal;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// xor-int/lit8 vAA, vBB, #+CC
void Interpreter::xor_int_lit8(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src = operand_[1];
	int8_t literal = static_cast<int8_t>(operand_[2]);
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src) ^ literal;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// shl-int/lit8 vAA, vBB, #+CC
void Interpreter::shl_int_lit8(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src = operand_[1];
	int8_t literal = static_cast<int8_t>(operand_[2]);
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src) << literal;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// shr-int/lit8 vAA, vBB, #+CC
void Interpreter::shr_int_lit8(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src = operand_[1];
	int8_t literal = static_cast<int8_t>(operand_[2]);
	auto& frame = _rt.currentFrame();
	int32_t result = frame.getIntRegister(src) >> literal;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
// ushr-int/lit8 vAA, vBB, #+CC
void Interpreter::ushr_int_lit8(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint8_t src = operand_[1];
	int8_t literal = static_cast<int8_t>(operand_[2]);
	auto& frame = _rt.currentFrame();
	uint32_t value = static_cast<uint32_t>(frame.getIntRegister(src));
	uint32_t result = value >> literal;
	frame.setIntRegister(dest, result);
	frame.pc() += 3;
}
