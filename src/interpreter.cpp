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
#include "vm.hpp"

using namespace sandvik;

Interpreter::Interpreter(JThread& rt_) : _rt(rt_) {
	_dispatch.resize(256, [](const uint8_t*) { throw VmException("Invalid instruction!"); });

	_dispatch[0x00] = std::bind_front(&Interpreter::nop, this);
	_dispatch[0x01] = std::bind_front(&Interpreter::move, this);
	_dispatch[0x02] = std::bind_front(&Interpreter::move_from16, this);
	_dispatch[0x03] = std::bind_front(&Interpreter::move_16, this);
	_dispatch[0x04] = std::bind_front(&Interpreter::move_wide, this);
	_dispatch[0x05] = std::bind_front(&Interpreter::move_wide_from16, this);
	_dispatch[0x06] = std::bind_front(&Interpreter::move_wide16, this);
	_dispatch[0x07] = std::bind_front(&Interpreter::move_object, this);
	_dispatch[0x08] = std::bind_front(&Interpreter::move_object_from16, this);
	_dispatch[0x09] = std::bind_front(&Interpreter::move_object16, this);
	_dispatch[0x0A] = std::bind_front(&Interpreter::move_result, this);
	_dispatch[0x0B] = std::bind_front(&Interpreter::move_result_wide, this);
	_dispatch[0x0C] = std::bind_front(&Interpreter::move_result_object, this);
	_dispatch[0x0D] = std::bind_front(&Interpreter::move_exception, this);
	_dispatch[0x0E] = std::bind_front(&Interpreter::return_void, this);
	_dispatch[0x0F] = std::bind_front(&Interpreter::return_, this);
	_dispatch[0x10] = std::bind_front(&Interpreter::return_wide, this);
	_dispatch[0x11] = std::bind_front(&Interpreter::return_object, this);
	_dispatch[0x12] = std::bind_front(&Interpreter::const_4, this);
	_dispatch[0x13] = std::bind_front(&Interpreter::const_16, this);
	_dispatch[0x14] = std::bind_front(&Interpreter::const_, this);
	_dispatch[0x15] = std::bind_front(&Interpreter::const_high16, this);
	_dispatch[0x16] = std::bind_front(&Interpreter::const_wide_16, this);
	_dispatch[0x17] = std::bind_front(&Interpreter::const_wide_32, this);
	_dispatch[0x18] = std::bind_front(&Interpreter::const_wide, this);
	_dispatch[0x19] = std::bind_front(&Interpreter::const_wide_high16, this);
	_dispatch[0x1A] = std::bind_front(&Interpreter::const_string, this);
	_dispatch[0x1B] = std::bind_front(&Interpreter::const_string_jumbo, this);
	_dispatch[0x1C] = std::bind_front(&Interpreter::const_class, this);
	_dispatch[0x1D] = std::bind_front(&Interpreter::monitor_enter, this);
	_dispatch[0x1E] = std::bind_front(&Interpreter::monitor_exit, this);
	_dispatch[0x1F] = std::bind_front(&Interpreter::check_cast, this);
	_dispatch[0x20] = std::bind_front(&Interpreter::instance_of, this);
	_dispatch[0x21] = std::bind_front(&Interpreter::array_length, this);
	_dispatch[0x22] = std::bind_front(&Interpreter::new_instance, this);
	_dispatch[0x23] = std::bind_front(&Interpreter::new_array, this);
	_dispatch[0x24] = std::bind_front(&Interpreter::filled_new_array, this);
	_dispatch[0x25] = std::bind_front(&Interpreter::filled_new_array_range, this);
	_dispatch[0x26] = std::bind_front(&Interpreter::fill_array_data, this);
	_dispatch[0x27] = std::bind_front(&Interpreter::throw_, this);
	_dispatch[0x28] = std::bind_front(&Interpreter::goto_, this);
	_dispatch[0x29] = std::bind_front(&Interpreter::goto_16, this);
	_dispatch[0x2A] = std::bind_front(&Interpreter::goto_32, this);
	_dispatch[0x2B] = std::bind_front(&Interpreter::packed_switch, this);
	_dispatch[0x2C] = std::bind_front(&Interpreter::sparse_switch, this);
	_dispatch[0x2D] = std::bind_front(&Interpreter::cmpl_float, this);
	_dispatch[0x2E] = std::bind_front(&Interpreter::cmpg_float, this);
	_dispatch[0x2F] = std::bind_front(&Interpreter::cmpl_double, this);
	_dispatch[0x30] = std::bind_front(&Interpreter::cmpg_double, this);
	_dispatch[0x31] = std::bind_front(&Interpreter::cmp_long, this);
	_dispatch[0x32] = std::bind_front(&Interpreter::if_eq, this);
	_dispatch[0x33] = std::bind_front(&Interpreter::if_ne, this);
	_dispatch[0x34] = std::bind_front(&Interpreter::if_lt, this);
	_dispatch[0x35] = std::bind_front(&Interpreter::if_ge, this);
	_dispatch[0x36] = std::bind_front(&Interpreter::if_gt, this);
	_dispatch[0x37] = std::bind_front(&Interpreter::if_le, this);
	_dispatch[0x38] = std::bind_front(&Interpreter::if_eqz, this);
	_dispatch[0x39] = std::bind_front(&Interpreter::if_nez, this);
	_dispatch[0x3A] = std::bind_front(&Interpreter::if_ltz, this);
	_dispatch[0x3B] = std::bind_front(&Interpreter::if_gez, this);
	_dispatch[0x3C] = std::bind_front(&Interpreter::if_gtz, this);
	_dispatch[0x3D] = std::bind_front(&Interpreter::if_lez, this);
	// 0x3E ... 0x43 (unused)
	_dispatch[0x44] = std::bind_front(&Interpreter::aget, this);
	_dispatch[0x45] = std::bind_front(&Interpreter::aget_wide, this);
	_dispatch[0x46] = std::bind_front(&Interpreter::aget_object, this);
	_dispatch[0x47] = std::bind_front(&Interpreter::aget_boolean, this);
	_dispatch[0x48] = std::bind_front(&Interpreter::aget_byte, this);
	_dispatch[0x49] = std::bind_front(&Interpreter::aget_char, this);
	_dispatch[0x4A] = std::bind_front(&Interpreter::aget_short, this);
	_dispatch[0x4B] = std::bind_front(&Interpreter::aput, this);
	_dispatch[0x4C] = std::bind_front(&Interpreter::aput_wide, this);
	_dispatch[0x4D] = std::bind_front(&Interpreter::aput_object, this);
	_dispatch[0x4E] = std::bind_front(&Interpreter::aput_boolean, this);
	_dispatch[0x4F] = std::bind_front(&Interpreter::aput_byte, this);
	_dispatch[0x50] = std::bind_front(&Interpreter::aput_char, this);
	_dispatch[0x51] = std::bind_front(&Interpreter::aput_short, this);
	_dispatch[0x52] = std::bind_front(&Interpreter::iget, this);
	_dispatch[0x53] = std::bind_front(&Interpreter::iget_wide, this);
	_dispatch[0x54] = std::bind_front(&Interpreter::iget_object, this);
	_dispatch[0x55] = std::bind_front(&Interpreter::iget_boolean, this);
	_dispatch[0x56] = std::bind_front(&Interpreter::iget_byte, this);
	_dispatch[0x57] = std::bind_front(&Interpreter::iget_char, this);
	_dispatch[0x58] = std::bind_front(&Interpreter::iget_short, this);
	_dispatch[0x59] = std::bind_front(&Interpreter::iput, this);
	_dispatch[0x5A] = std::bind_front(&Interpreter::iput_wide, this);
	_dispatch[0x5B] = std::bind_front(&Interpreter::iput_object, this);
	_dispatch[0x5C] = std::bind_front(&Interpreter::iput_boolean, this);
	_dispatch[0x5D] = std::bind_front(&Interpreter::iput_byte, this);
	_dispatch[0x5E] = std::bind_front(&Interpreter::iput_char, this);
	_dispatch[0x5F] = std::bind_front(&Interpreter::iput_short, this);
	_dispatch[0x60] = std::bind_front(&Interpreter::sget, this);
	_dispatch[0x61] = std::bind_front(&Interpreter::sget_wide, this);
	_dispatch[0x62] = std::bind_front(&Interpreter::sget_object, this);
	_dispatch[0x63] = std::bind_front(&Interpreter::sget_boolean, this);
	_dispatch[0x64] = std::bind_front(&Interpreter::sget_byte, this);
	_dispatch[0x65] = std::bind_front(&Interpreter::sget_char, this);
	_dispatch[0x66] = std::bind_front(&Interpreter::sget_short, this);
	_dispatch[0x67] = std::bind_front(&Interpreter::sput, this);
	_dispatch[0x68] = std::bind_front(&Interpreter::sput_wide, this);
	_dispatch[0x69] = std::bind_front(&Interpreter::sput_object, this);
	_dispatch[0x6A] = std::bind_front(&Interpreter::sput_boolean, this);
	_dispatch[0x6B] = std::bind_front(&Interpreter::sput_byte, this);
	_dispatch[0x6C] = std::bind_front(&Interpreter::sput_char, this);
	_dispatch[0x6D] = std::bind_front(&Interpreter::sput_short, this);
	_dispatch[0x6E] = std::bind_front(&Interpreter::invoke_virtual, this);
	_dispatch[0x6F] = std::bind_front(&Interpreter::invoke_super, this);
	_dispatch[0x70] = std::bind_front(&Interpreter::invoke_direct, this);
	_dispatch[0x71] = std::bind_front(&Interpreter::invoke_static, this);
	_dispatch[0x72] = std::bind_front(&Interpreter::invoke_interface, this);
	// 73 unused
	_dispatch[0x74] = std::bind_front(&Interpreter::invoke_virtual_range, this);
	_dispatch[0x75] = std::bind_front(&Interpreter::invoke_super_range, this);
	_dispatch[0x76] = std::bind_front(&Interpreter::invoke_direct_range, this);
	_dispatch[0x77] = std::bind_front(&Interpreter::invoke_static_range, this);
	_dispatch[0x78] = std::bind_front(&Interpreter::invoke_interface_range, this);
	// 0x79 ... 0x7A (unused)
	_dispatch[0x7B] = std::bind_front(&Interpreter::neg_int, this);
	_dispatch[0x7C] = std::bind_front(&Interpreter::not_int, this);
	_dispatch[0x7D] = std::bind_front(&Interpreter::neg_long, this);
	_dispatch[0x7E] = std::bind_front(&Interpreter::not_long, this);
	_dispatch[0x7F] = std::bind_front(&Interpreter::neg_float, this);
	_dispatch[0x80] = std::bind_front(&Interpreter::neg_double, this);
	_dispatch[0x81] = std::bind_front(&Interpreter::int_to_long, this);
	_dispatch[0x82] = std::bind_front(&Interpreter::int_to_float, this);
	_dispatch[0x83] = std::bind_front(&Interpreter::int_to_double, this);
	_dispatch[0x84] = std::bind_front(&Interpreter::long_to_int, this);
	_dispatch[0x85] = std::bind_front(&Interpreter::long_to_float, this);
	_dispatch[0x86] = std::bind_front(&Interpreter::long_to_double, this);
	_dispatch[0x87] = std::bind_front(&Interpreter::float_to_int, this);
	_dispatch[0x88] = std::bind_front(&Interpreter::float_to_long, this);
	_dispatch[0x89] = std::bind_front(&Interpreter::float_to_double, this);
	_dispatch[0x8A] = std::bind_front(&Interpreter::double_to_int, this);
	_dispatch[0x8B] = std::bind_front(&Interpreter::double_to_long, this);
	_dispatch[0x8C] = std::bind_front(&Interpreter::double_to_float, this);
	_dispatch[0x8D] = std::bind_front(&Interpreter::int_to_byte, this);
	_dispatch[0x8E] = std::bind_front(&Interpreter::int_to_char, this);
	_dispatch[0x8F] = std::bind_front(&Interpreter::int_to_short, this);
	_dispatch[0x90] = std::bind_front(&Interpreter::add_int, this);
	_dispatch[0x91] = std::bind_front(&Interpreter::sub_int, this);
	_dispatch[0x92] = std::bind_front(&Interpreter::mul_int, this);
	_dispatch[0x93] = std::bind_front(&Interpreter::div_int, this);
	_dispatch[0x94] = std::bind_front(&Interpreter::rem_int, this);
	_dispatch[0x95] = std::bind_front(&Interpreter::and_int, this);
	_dispatch[0x96] = std::bind_front(&Interpreter::or_int, this);
	_dispatch[0x97] = std::bind_front(&Interpreter::xor_int, this);
	_dispatch[0x98] = std::bind_front(&Interpreter::shl_int, this);
	_dispatch[0x99] = std::bind_front(&Interpreter::shr_int, this);
	_dispatch[0x9A] = std::bind_front(&Interpreter::ushr_int, this);
	_dispatch[0x9B] = std::bind_front(&Interpreter::add_long, this);
	_dispatch[0x9C] = std::bind_front(&Interpreter::sub_long, this);
	_dispatch[0x9D] = std::bind_front(&Interpreter::mul_long, this);
	_dispatch[0x9E] = std::bind_front(&Interpreter::div_long, this);
	_dispatch[0x9F] = std::bind_front(&Interpreter::rem_long, this);
	_dispatch[0xA0] = std::bind_front(&Interpreter::and_long, this);
	_dispatch[0xA1] = std::bind_front(&Interpreter::or_long, this);
	_dispatch[0xA2] = std::bind_front(&Interpreter::xor_long, this);
	_dispatch[0xA3] = std::bind_front(&Interpreter::shl_long, this);
	_dispatch[0xA4] = std::bind_front(&Interpreter::shr_long, this);
	_dispatch[0xA5] = std::bind_front(&Interpreter::ushr_long, this);
	_dispatch[0xA6] = std::bind_front(&Interpreter::add_float, this);
	_dispatch[0xA7] = std::bind_front(&Interpreter::sub_float, this);
	_dispatch[0xA8] = std::bind_front(&Interpreter::mul_float, this);
	_dispatch[0xA9] = std::bind_front(&Interpreter::div_float, this);
	_dispatch[0xAA] = std::bind_front(&Interpreter::rem_float, this);
	_dispatch[0xAB] = std::bind_front(&Interpreter::add_double, this);
	_dispatch[0xAC] = std::bind_front(&Interpreter::sub_double, this);
	_dispatch[0xAD] = std::bind_front(&Interpreter::mul_double, this);
	_dispatch[0xAE] = std::bind_front(&Interpreter::div_double, this);
	_dispatch[0xAF] = std::bind_front(&Interpreter::rem_double, this);
	_dispatch[0xB0] = std::bind_front(&Interpreter::add_int_2addr, this);
	_dispatch[0xB1] = std::bind_front(&Interpreter::sub_int_2addr, this);
	_dispatch[0xB2] = std::bind_front(&Interpreter::mul_int_2addr, this);
	_dispatch[0xB3] = std::bind_front(&Interpreter::div_int_2addr, this);
	_dispatch[0xB4] = std::bind_front(&Interpreter::rem_int_2addr, this);
	_dispatch[0xB5] = std::bind_front(&Interpreter::and_int_2addr, this);
	_dispatch[0xB6] = std::bind_front(&Interpreter::or_int_2addr, this);
	_dispatch[0xB7] = std::bind_front(&Interpreter::xor_int_2addr, this);
	_dispatch[0xB8] = std::bind_front(&Interpreter::shl_int_2addr, this);
	_dispatch[0xB9] = std::bind_front(&Interpreter::shr_int_2addr, this);
	_dispatch[0xBA] = std::bind_front(&Interpreter::ushr_int_2addr, this);
	_dispatch[0xBB] = std::bind_front(&Interpreter::add_long_2addr, this);
	_dispatch[0xBC] = std::bind_front(&Interpreter::sub_long_2addr, this);
	_dispatch[0xBD] = std::bind_front(&Interpreter::mul_long_2addr, this);
	_dispatch[0xBE] = std::bind_front(&Interpreter::div_long_2addr, this);
	_dispatch[0xBF] = std::bind_front(&Interpreter::rem_long_2addr, this);
	_dispatch[0xC0] = std::bind_front(&Interpreter::and_long_2addr, this);
	_dispatch[0xC1] = std::bind_front(&Interpreter::or_long_2addr, this);
	_dispatch[0xC2] = std::bind_front(&Interpreter::xor_long_2addr, this);
	_dispatch[0xC3] = std::bind_front(&Interpreter::shl_long_2addr, this);
	_dispatch[0xC4] = std::bind_front(&Interpreter::shr_long_2addr, this);
	_dispatch[0xC5] = std::bind_front(&Interpreter::ushr_long_2addr, this);
	_dispatch[0xC6] = std::bind_front(&Interpreter::add_float_2addr, this);
	_dispatch[0xC7] = std::bind_front(&Interpreter::sub_float_2addr, this);
	_dispatch[0xC8] = std::bind_front(&Interpreter::mul_float_2addr, this);
	_dispatch[0xC9] = std::bind_front(&Interpreter::div_float_2addr, this);
	_dispatch[0xCA] = std::bind_front(&Interpreter::rem_float_2addr, this);
	_dispatch[0xCB] = std::bind_front(&Interpreter::add_double_2addr, this);
	_dispatch[0xCC] = std::bind_front(&Interpreter::sub_double_2addr, this);
	_dispatch[0xCD] = std::bind_front(&Interpreter::mul_double_2addr, this);
	_dispatch[0xCE] = std::bind_front(&Interpreter::div_double_2addr, this);
	_dispatch[0xCF] = std::bind_front(&Interpreter::rem_double_2addr, this);
	_dispatch[0xD0] = std::bind_front(&Interpreter::add_int_lit16, this);
	_dispatch[0xD1] = std::bind_front(&Interpreter::rsub_int_lit16, this);
	_dispatch[0xD2] = std::bind_front(&Interpreter::mul_int_lit16, this);
	_dispatch[0xD3] = std::bind_front(&Interpreter::div_int_lit16, this);
	_dispatch[0xD4] = std::bind_front(&Interpreter::rem_int_lit16, this);
	_dispatch[0xD5] = std::bind_front(&Interpreter::and_int_lit16, this);
	_dispatch[0xD6] = std::bind_front(&Interpreter::or_int_lit16, this);
	_dispatch[0xD7] = std::bind_front(&Interpreter::xor_int_lit16, this);
	_dispatch[0xD8] = std::bind_front(&Interpreter::add_int_lit8, this);
	_dispatch[0xD9] = std::bind_front(&Interpreter::rsub_int_lit8, this);
	_dispatch[0xDA] = std::bind_front(&Interpreter::mul_int_lit8, this);
	_dispatch[0xDB] = std::bind_front(&Interpreter::div_int_lit8, this);
	_dispatch[0xDC] = std::bind_front(&Interpreter::rem_int_lit8, this);
	_dispatch[0xDD] = std::bind_front(&Interpreter::and_int_lit8, this);
	_dispatch[0xDE] = std::bind_front(&Interpreter::or_int_lit8, this);
	_dispatch[0xDF] = std::bind_front(&Interpreter::xor_int_lit8, this);
	_dispatch[0xE0] = std::bind_front(&Interpreter::shl_int_lit8, this);
	_dispatch[0xE1] = std::bind_front(&Interpreter::shr_int_lit8, this);
	_dispatch[0xE2] = std::bind_front(&Interpreter::ushr_int_lit8, this);
	// 0xE3 ... 0xFF (unused)
}

Interpreter::~Interpreter() {
#if 0
	/*std::vector<std::pair<uint8_t, uint64_t>> sortedCoverage(_instcoverage.begin(), _instcoverage.end());
	std::sort(sortedCoverage.begin(), sortedCoverage.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
	for (const auto& [key, value] : sortedCoverage) {
	    logger.finfo("Instruction 0x{:02x} executed {} times", key, value);
	}*/
	auto count = 0;
	for (uint16_t i = 0; i < 0xE3; ++i) {
		if (_instcoverage.find(i) == _instcoverage.end()) {
			auto s = _disassembler->disassemble(i);
			if (!s.empty()) {
				logger.fwarning("Missing coverage for instruction 0x{:02x} {}", i, s);
				count++;
			}
		}
	}
	logger.finfo("{} tested, missing coverage for {} instructions", _instcoverage.size(), count - 8);
#endif
}

void Interpreter::execute() {
	auto& frame = _rt.currentFrame();
	const auto& method = frame.getMethod();
	auto code = method.getBytecode();
	auto func = fmt::format("{}::{}{}", method.getClass().getFullname(), method.getName(), method.getSignature());
	if (code == nullptr) {
		throw VmException("Method {} has no bytecode!", func);
	}
	if (frame.pc() >= method.getBytecodeSize()) {
		throw VmException("Current frame {} has invalid pc: {}", func, frame.pc());
	}
	auto bytecode = code + frame.pc();
	trace.logInstruction(frame.pc(), func, bytecode);
	frame.pc()++;
	_instcoverage[*bytecode]++;
	try {
		_dispatch[*bytecode](bytecode + 1);
	} catch (JavaException& e) {
		auto exctype = e.getExceptionType();
		logger.fdebug("handling exception {} ({}) in method {}", exctype, e.what(), func);
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
	// Create a new thread to execute the <clinit> method
	auto& thread = _rt.vm().newThread(fmt::format("{}.{}", class_.getFullname(), "<clinit>"));
	if (hasInitializeMethod) {
		// push new frame with initializeSystemClass method (should be executed after <clinit>)
		auto& initializeSystemClass = class_.getMethod("initializeSystemClass", "()V");
		thread.newFrame(initializeSystemClass);
	}
	if (hasClinitMethod) {
		// push new frame with <clinit> method
		auto& clinitMethod = class_.getMethod("<clinit>", "()V");
		if (clinitMethod.isNative()) {
			throw VmException("Native <clinit> method for class {} is not supported!", class_.getFullname());
		}
		if (!clinitMethod.hasBytecode()) {
			throw VmException("<clinit> method for class {} has no bytecode!", class_.getFullname());
		}
		thread.newFrame(clinitMethod);
	}
	thread.run(true);
	thread.join();
}

void Interpreter::executeNativeMethod(const Method& method_, const std::vector<std::shared_ptr<Object>>& args_) {
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
	auto caller = std::make_unique<NativeCallHelper>(*_rt.vm().getJNIEnv());
	auto ret = caller->invoke(symbol, _rt.vm().getJNIEnv(), args_, returnType, params, method_.isStatic());
	_rt.currentFrame().setReturnObject(ret);
}

void Interpreter::handleException(std::shared_ptr<Object> exception_) {
	auto exception = exception_;
	if (!exception->isClass()) {
		throw VmException("throw operand is not an object!");
	}

	while (true) {
		try {
			auto& frame = _rt.currentFrame();
			const auto& method = frame.getMethod();
			uint32_t catchAllAddrress = 0;
			auto exceptionHandler = method.getExceptionHandler(frame.pc() - 1, catchAllAddrress);
			for (auto& exc : exceptionHandler) {
				auto& classloader = _rt.getClassLoader();
				const auto& exceptionType = classloader.resolveClass(frame.getDexIdx(), exc.first);
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
		if (_rt.stackDepth() == 0) {
			// uncaught exception
			auto detailMessage = exception->getField("detailMessage");
			std::string msg = detailMessage->isString() ? detailMessage->str() : "";
			logger.ferror("Unhandled exception {} : {}", exception->getClass().getFullname(), msg);
			throw JavaException(exception->getClass().getFullname(), msg);
		}
		_rt.currentFrame().setException(exception_);
	}
}

std::vector<std::shared_ptr<Object>> Interpreter::getInvokeMethodArgs(const uint8_t* operand_) const {
	auto& frame = _rt.currentFrame();
	const uint8_t vA = (operand_[0] >> 4) & 0x0F;  // Number of registers (A)
	uint8_t vC = (vA > 0) ? operand_[3] & 0x0F : 0;
	uint8_t vD = (vA > 0) ? (operand_[3] >> 4) & 0x0F : 0;
	uint8_t vE = (vA > 1) ? operand_[4] & 0x0F : 0;
	uint8_t vF = (vA > 1) ? (operand_[4] >> 4) & 0x0F : 0;
	uint8_t vG = (vA > 2) ? operand_[0] & 0x0F : 0;

	std::vector<uint8_t> regs = {vC, vD, vE, vF, vG};
	std::vector<std::shared_ptr<Object>> args{};
	for (uint8_t i = 0; i < vA; ++i) {
		auto obj = frame.getObjRegister(regs[i]);
		args.push_back(obj);
	}
	return args;
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
	_rt.popFrame();
	if (_rt.end()) {
		// main method return
		_rt.setReturnObject(ret);
		return;
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
	if (obj->isNull()) {
		throw NullPointerException("check_cast on null object");
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
			auto array = std::dynamic_pointer_cast<Array>(obj);
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
		throw NullPointerException("array_length on null object");
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
	frame.setObjRegister(dest, Object::make(cls));
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
	const auto& type = classloader.getOrLoad(arrayType[0].first);
	auto arrayObj = Array::make(type, size);
	frame.setObjRegister(dest, arrayObj);
	frame.pc() += 3;
}
// filled-new-array {vD, vE, vF, vG, vA}, type@CCCC
void Interpreter::filled_new_array(const uint8_t* operand_) {
	throw VmException("filled_new_array not implemented");
}
// filled-new-array/range {vCCCC .. vNNNN}, type@BBBB
void Interpreter::filled_new_array_range(const uint8_t* operand_) {
	throw VmException("filled_new_array_range not implemented");
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

	auto array = std::dynamic_pointer_cast<Array>(arrayObj);
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
	if (obj && obj->isNumberObject()) {
		if (frame.getIntRegister(regA) == 0) {
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
	if (obj && obj->isNumberObject()) {
		if (frame.getIntRegister(regA) != 0) {
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
	auto array = std::dynamic_pointer_cast<Array>(arrayObj);
	if (!array) {
		throw ClassCastException("aget: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aget: Array index out of bounds");
	}
	auto obj = array->getElement(index);
	int32_t value = 0;
	if (!obj->isNumberObject()) {
		if (!obj->isNull()) {
			throw VmException("aget: Array does not contain number");
		}
	} else {
		value = obj->getValue();
	}
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
	auto array = std::dynamic_pointer_cast<Array>(arrayObj);
	if (!array) {
		throw ClassCastException("aget-wide: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aget-wide: Array index out of bounds");
	}
	auto element = array->getElement(index);
	if (!element->isNumberObject()) {
		throw VmException("aget-wide: Array element is not a number object");
	}
	int64_t value = element->getLongValue();
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
	auto array = std::dynamic_pointer_cast<Array>(arrayObj);
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
	auto array = std::dynamic_pointer_cast<Array>(arrayObj);
	if (!array) {
		throw ClassCastException("aget-boolean: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aget-boolean: Array index out of bounds");
	}
	auto element = array->getElement(index);
	if (!element->isNumberObject()) {
		throw VmException("aget-boolean: Array element is not a number object");
	}
	bool value = element->getValue() != 0;
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
	auto array = std::dynamic_pointer_cast<Array>(arrayObj);
	if (!array) {
		throw ClassCastException("aget-byte: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aget-byte: Array index out of bounds");
	}
	auto element = array->getElement(index);
	if (!element->isNumberObject()) {
		throw VmException("aget-byte: Array element is not a number object");
	}
	int8_t value = static_cast<int8_t>(element->getValue());
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
	auto array = std::dynamic_pointer_cast<Array>(arrayObj);
	if (!array) {
		throw ClassCastException("aget-char: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aget-char: Array index out of bounds");
	}
	auto element = array->getElement(index);
	if (!element->isNumberObject()) {
		throw VmException("aget-char: Array element is not a number object");
	}
	uint16_t value = static_cast<uint16_t>(element->getValue());
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
	auto array = std::dynamic_pointer_cast<Array>(arrayObj);
	if (!array) {
		throw ClassCastException("aget-short: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aget-short: Array index out of bounds");
	}
	auto element = array->getElement(index);
	if (!element->isNumberObject()) {
		throw VmException("aget-short: Array element is not a number object");
	}
	int16_t value = static_cast<int16_t>(element->getValue());
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
	auto array = std::dynamic_pointer_cast<Array>(arrayObj);
	if (!array) {
		throw ClassCastException("aput: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	int32_t value = frame.getIntRegister(valueReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aput: Array index out of bounds");
	}
	array->setElement(index, Object::make(value));
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
	auto array = std::dynamic_pointer_cast<Array>(arrayObj);
	if (!array) {
		throw ClassCastException("aput-wide: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aput-wide: Array index out of bounds");
	}
	auto value = frame.getLongRegister(valueReg);
	array->setElement(index, Object::make(value));
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
	auto array = std::dynamic_pointer_cast<Array>(arrayObj);
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
	auto array = std::dynamic_pointer_cast<Array>(arrayObj);
	if (!array) {
		throw ClassCastException("aput-boolean: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	bool value = frame.getIntRegister(valueReg) != 0;
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aput-boolean: Array index out of bounds");
	}
	array->setElement(index, Object::make(value));
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
	auto array = std::dynamic_pointer_cast<Array>(arrayObj);
	if (!array) {
		throw ClassCastException("aput-byte: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	int8_t value = static_cast<int8_t>(frame.getIntRegister(valueReg));
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aput-byte: Array index out of bounds");
	}
	array->setElement(index, Object::make(value));
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
	auto array = std::dynamic_pointer_cast<Array>(arrayObj);
	if (!array) {
		throw ClassCastException("aput-char: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	uint16_t value = static_cast<uint16_t>(frame.getIntRegister(valueReg));
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aput-char: Array index out of bounds");
	}
	array->setElement(index, Object::make(value));
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
	auto array = std::dynamic_pointer_cast<Array>(arrayObj);
	if (!array) {
		throw ClassCastException("aput-short: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	int16_t value = static_cast<int16_t>(frame.getIntRegister(valueReg));
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("aput-short: Array index out of bounds");
	}
	array->setElement(index, Object::make(value));
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
	bool value = static_cast<bool>(fieldObj->getValue());
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
	obj->setField(field.getName(), Object::make(value));
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
	obj->setField(field.getName(), Object::make(value));
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

	bool value = frame.getIntRegister(src) != 0;
	logger.fdebug("iput_boolean {}.{}={}", field.getClass().getFullname(), field.getName(), value);
	obj->setField(field.getName(), Object::make(value));
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
	obj->setField(field.getName(), Object::make(value));
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
	obj->setField(field.getName(), Object::make(value));
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
	obj->setField(field.getName(), Object::make(value));
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
	logger.fdebug("sget_object: Resolving field {}", field.str());
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
	bool value = field.getIntValue() != 0;
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
		throw VmException("sget_wide: Cannot use sget on a non-static field");
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
	// @todo : this is a hack to handle setting null to an object field. Need to properly handle this case.
	if (value->isNumberObject() && (value->getValue() == 0)) {
		auto& fieldclass = classloader.getOrLoad(field.getFieldTypeClassname());
		auto obj = Object::make(fieldclass);
		field.setObjectValue(obj);
	} else {
		field.setObjectValue(value);
	}

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

	bool value = frame.getIntRegister(src) != 0;
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
		throw NullPointerException("invoke-virtual on null object");
	}
	if (!this_ptr->isClass()) {
		throw VmException("invoke-virtual: this pointer is not an ObjectClass, got {}", this_ptr->toString());
	}
	Class* instance = &this_ptr->getClass();
	if (!instance->isStaticInitialized()) {
		throw VmException("invoke-virtual: class {} is not static initialized", instance->getFullname());
	}

	std::string classname, methodname, signature;
	classloader.findMethod(frame.getDexIdx(), methodRef, classname, methodname, signature);

	Method* vmethod = nullptr;
	while (true) {
		try {
			vmethod = &instance->getMethod(methodname, signature);
			break;  // Method found, exit loop
		} catch (std::exception& e) {
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
				for (auto i = 0; i < args.size(); i++) {
					newframe.setObjRegister(vmethod->getNbRegisters() - args.size() + i, args[i]);
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
		try {
			vmethod = &instance->getMethod(methodname, signature);
			break;  // Method found, exit loop
		} catch (std::exception& e) {
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
	}

	auto args = getInvokeMethodArgs(operand_);
	trace.logCall("invoke-super", instance->getFullname(), methodname, signature, args, vmethod->isStatic());
	if (vmethod->isNative()) {
		executeNativeMethod(*vmethod, args);
	} else {
		if (vmethod->getBytecode() == nullptr) {
			vmethod->execute(frame, args);
		} else {
			auto& newframe = _rt.newFrame(*vmethod);
			// set args on new frame
			// When a method is invoked, the parameters to the method are placed into the last n registers.
			for (auto i = 0; i < args.size(); i++) {
				newframe.setObjRegister(vmethod->getNbRegisters() - args.size() + i, args[i]);
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
	if (method.isNative()) {
		executeNativeMethod(method, args);
	} else {
		if (method.getBytecode() == nullptr) {
			method.execute(frame, args);
		} else {
			auto& newframe = _rt.newFrame(method);
			// set args on new frame
			// When a method is invoked, the parameters to the method are placed into the last n registers.
			for (auto i = 0; i < args.size(); i++) {
				newframe.setObjRegister(method.getNbRegisters() - args.size() + i, args[i]);
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
		throw VmException("invoke-interface: class {} is not static initialized", instance->getFullname());
	}

	std::string ifclassname, methodname, signature;
	classloader.findMethod(frame.getDexIdx(), methodRef, ifclassname, methodname, signature);

	logger.fdebug("invoke-interface {}->{}{} for class {}", ifclassname, methodname, signature, instance->getFullname());

	Method* vmethod = nullptr;
	Class* current = instance;
	while (current) {
		try {
			vmethod = &current->getMethod(methodname, signature);
			break;  // Method found, exit loop
		} catch (...) {
			// not found in this class
			if (current->hasSuperClass()) {
				current = &classloader.getOrLoad(current->getSuperClassname());
				if (!current->isStaticInitialized()) {
					executeClinit(*current);
				}
			} else {
				current = nullptr;
			}
		}
	}
	if (vmethod) {
		if (!vmethod->isVirtual()) {
			logger.ferror("invoke-interface: {}->{}{} not virtual", ifclassname, methodname, signature);
		}
		trace.logCall("invoke-interface", ifclassname, methodname, signature, args, vmethod->isStatic());
		if (vmethod->isNative()) {
			executeNativeMethod(*vmethod, args);
		} else {
			auto& newframe = _rt.newFrame(*vmethod);
			// When a method is invoked, the parameters to the method are placed into the last n registers.
			for (auto i = 0; i < args.size(); i++) {
				newframe.setObjRegister(vmethod->getNbRegisters() - args.size() + i, args[i]);
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

	std::vector<std::shared_ptr<Object>> args;
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

	std::string classname, methodname, signature;
	classloader.findMethod(frame.getDexIdx(), methodRef, classname, methodname, signature);

	Method* vmethod = nullptr;
	while (true) {
		try {
			vmethod = &instance->getMethod(methodname, signature);
			break;  // Method found, exit loop
		} catch (std::exception& e) {
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
				for (auto i = 0; i < args.size(); i++) {
					newframe.setObjRegister(vmethod->getNbRegisters() - args.size() + i, args[i]);
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

	std::vector<std::shared_ptr<Object>> args;
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
		if (method.getBytecode() == nullptr) {
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
	throw VmException("invoke_interface_range not implemented");
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
