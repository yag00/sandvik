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
#include "vm.hpp"

using namespace sandvik;

Interpreter::Interpreter(JThread& rt_) : _rt(rt_), _disassembler(std::make_unique<Disassembler>()) {
	_dispatch.resize(256, [](const uint8_t* operand_) { throw std::runtime_error("Invalid instruction!"); });

	_dispatch[0x00] = std::bind(&Interpreter::nop, this, std::placeholders::_1);
	_dispatch[0x01] = std::bind(&Interpreter::move, this, std::placeholders::_1);
	_dispatch[0x02] = std::bind(&Interpreter::move_from16, this, std::placeholders::_1);
	_dispatch[0x03] = std::bind(&Interpreter::move_16, this, std::placeholders::_1);
	_dispatch[0x04] = std::bind(&Interpreter::move_wide, this, std::placeholders::_1);
	_dispatch[0x05] = std::bind(&Interpreter::move_wide_from16, this, std::placeholders::_1);
	_dispatch[0x06] = std::bind(&Interpreter::move_wide16, this, std::placeholders::_1);
	_dispatch[0x07] = std::bind(&Interpreter::move_object, this, std::placeholders::_1);
	_dispatch[0x08] = std::bind(&Interpreter::move_object_from16, this, std::placeholders::_1);
	_dispatch[0x09] = std::bind(&Interpreter::move_object16, this, std::placeholders::_1);
	_dispatch[0x0A] = std::bind(&Interpreter::move_result, this, std::placeholders::_1);
	_dispatch[0x0B] = std::bind(&Interpreter::move_result_wide, this, std::placeholders::_1);
	_dispatch[0x0C] = std::bind(&Interpreter::move_result_object, this, std::placeholders::_1);
	_dispatch[0x0D] = std::bind(&Interpreter::move_exception, this, std::placeholders::_1);
	_dispatch[0x0E] = std::bind(&Interpreter::return_void, this, std::placeholders::_1);
	_dispatch[0x0F] = std::bind(&Interpreter::return_, this, std::placeholders::_1);
	_dispatch[0x10] = std::bind(&Interpreter::return_wide, this, std::placeholders::_1);
	_dispatch[0x11] = std::bind(&Interpreter::return_object, this, std::placeholders::_1);
	_dispatch[0x12] = std::bind(&Interpreter::const_4, this, std::placeholders::_1);
	_dispatch[0x13] = std::bind(&Interpreter::const_16, this, std::placeholders::_1);
	_dispatch[0x14] = std::bind(&Interpreter::const_, this, std::placeholders::_1);
	_dispatch[0x15] = std::bind(&Interpreter::const_high16, this, std::placeholders::_1);
	_dispatch[0x16] = std::bind(&Interpreter::const_wide_16, this, std::placeholders::_1);
	_dispatch[0x17] = std::bind(&Interpreter::const_wide_32, this, std::placeholders::_1);
	_dispatch[0x18] = std::bind(&Interpreter::const_wide, this, std::placeholders::_1);
	_dispatch[0x19] = std::bind(&Interpreter::const_wide_high16, this, std::placeholders::_1);
	_dispatch[0x1A] = std::bind(&Interpreter::const_string, this, std::placeholders::_1);
	_dispatch[0x1B] = std::bind(&Interpreter::const_string_jumbo, this, std::placeholders::_1);
	_dispatch[0x1C] = std::bind(&Interpreter::const_class, this, std::placeholders::_1);
	_dispatch[0x1D] = std::bind(&Interpreter::monitor_enter, this, std::placeholders::_1);
	_dispatch[0x1E] = std::bind(&Interpreter::monitor_exit, this, std::placeholders::_1);
	_dispatch[0x1F] = std::bind(&Interpreter::check_cast, this, std::placeholders::_1);
	_dispatch[0x20] = std::bind(&Interpreter::instance_of, this, std::placeholders::_1);
	_dispatch[0x21] = std::bind(&Interpreter::array_length, this, std::placeholders::_1);
	_dispatch[0x22] = std::bind(&Interpreter::new_instance, this, std::placeholders::_1);
	_dispatch[0x23] = std::bind(&Interpreter::new_array, this, std::placeholders::_1);
	_dispatch[0x24] = std::bind(&Interpreter::filled_new_array, this, std::placeholders::_1);
	_dispatch[0x25] = std::bind(&Interpreter::filled_new_array_range, this, std::placeholders::_1);
	_dispatch[0x26] = std::bind(&Interpreter::fill_array_data, this, std::placeholders::_1);
	_dispatch[0x27] = std::bind(&Interpreter::throw_, this, std::placeholders::_1);
	_dispatch[0x28] = std::bind(&Interpreter::goto_, this, std::placeholders::_1);
	_dispatch[0x29] = std::bind(&Interpreter::goto_16, this, std::placeholders::_1);
	_dispatch[0x2A] = std::bind(&Interpreter::goto_32, this, std::placeholders::_1);
	_dispatch[0x2B] = std::bind(&Interpreter::packed_switch, this, std::placeholders::_1);
	_dispatch[0x2C] = std::bind(&Interpreter::sparse_switch, this, std::placeholders::_1);
	_dispatch[0x2D] = std::bind(&Interpreter::cmpl_float, this, std::placeholders::_1);
	_dispatch[0x2E] = std::bind(&Interpreter::cmpg_float, this, std::placeholders::_1);
	_dispatch[0x2F] = std::bind(&Interpreter::cmpl_double, this, std::placeholders::_1);
	_dispatch[0x30] = std::bind(&Interpreter::cmpg_double, this, std::placeholders::_1);
	_dispatch[0x31] = std::bind(&Interpreter::cmp_long, this, std::placeholders::_1);
	_dispatch[0x32] = std::bind(&Interpreter::if_eq, this, std::placeholders::_1);
	_dispatch[0x33] = std::bind(&Interpreter::if_ne, this, std::placeholders::_1);
	_dispatch[0x34] = std::bind(&Interpreter::if_lt, this, std::placeholders::_1);
	_dispatch[0x35] = std::bind(&Interpreter::if_ge, this, std::placeholders::_1);
	_dispatch[0x36] = std::bind(&Interpreter::if_gt, this, std::placeholders::_1);
	_dispatch[0x37] = std::bind(&Interpreter::if_le, this, std::placeholders::_1);
	_dispatch[0x38] = std::bind(&Interpreter::if_eqz, this, std::placeholders::_1);
	_dispatch[0x39] = std::bind(&Interpreter::if_nez, this, std::placeholders::_1);
	_dispatch[0x3A] = std::bind(&Interpreter::if_ltz, this, std::placeholders::_1);
	_dispatch[0x3B] = std::bind(&Interpreter::if_gez, this, std::placeholders::_1);
	_dispatch[0x3C] = std::bind(&Interpreter::if_gtz, this, std::placeholders::_1);
	_dispatch[0x3D] = std::bind(&Interpreter::if_lez, this, std::placeholders::_1);
	// 0x3E ... 0x43 (unused)
	_dispatch[0x44] = std::bind(&Interpreter::aget, this, std::placeholders::_1);
	_dispatch[0x45] = std::bind(&Interpreter::aget_wide, this, std::placeholders::_1);
	_dispatch[0x46] = std::bind(&Interpreter::aget_object, this, std::placeholders::_1);
	_dispatch[0x47] = std::bind(&Interpreter::aget_boolean, this, std::placeholders::_1);
	_dispatch[0x48] = std::bind(&Interpreter::aget_byte, this, std::placeholders::_1);
	_dispatch[0x49] = std::bind(&Interpreter::aget_char, this, std::placeholders::_1);
	_dispatch[0x4A] = std::bind(&Interpreter::aget_short, this, std::placeholders::_1);
	_dispatch[0x4B] = std::bind(&Interpreter::aput, this, std::placeholders::_1);
	_dispatch[0x4C] = std::bind(&Interpreter::aput_wide, this, std::placeholders::_1);
	_dispatch[0x4D] = std::bind(&Interpreter::aput_object, this, std::placeholders::_1);
	_dispatch[0x4E] = std::bind(&Interpreter::aput_boolean, this, std::placeholders::_1);
	_dispatch[0x4F] = std::bind(&Interpreter::aput_byte, this, std::placeholders::_1);
	_dispatch[0x50] = std::bind(&Interpreter::aput_char, this, std::placeholders::_1);
	_dispatch[0x51] = std::bind(&Interpreter::aput_short, this, std::placeholders::_1);
	_dispatch[0x52] = std::bind(&Interpreter::iget, this, std::placeholders::_1);
	_dispatch[0x53] = std::bind(&Interpreter::iget_wide, this, std::placeholders::_1);
	_dispatch[0x54] = std::bind(&Interpreter::iget_object, this, std::placeholders::_1);
	_dispatch[0x55] = std::bind(&Interpreter::iget_boolean, this, std::placeholders::_1);
	_dispatch[0x56] = std::bind(&Interpreter::iget_byte, this, std::placeholders::_1);
	_dispatch[0x57] = std::bind(&Interpreter::iget_char, this, std::placeholders::_1);
	_dispatch[0x58] = std::bind(&Interpreter::iget_short, this, std::placeholders::_1);
	_dispatch[0x59] = std::bind(&Interpreter::iput, this, std::placeholders::_1);
	_dispatch[0x5A] = std::bind(&Interpreter::iput_wide, this, std::placeholders::_1);
	_dispatch[0x5B] = std::bind(&Interpreter::iput_object, this, std::placeholders::_1);
	_dispatch[0x5C] = std::bind(&Interpreter::iput_boolean, this, std::placeholders::_1);
	_dispatch[0x5D] = std::bind(&Interpreter::iput_byte, this, std::placeholders::_1);
	_dispatch[0x5E] = std::bind(&Interpreter::iput_char, this, std::placeholders::_1);
	_dispatch[0x5F] = std::bind(&Interpreter::iput_short, this, std::placeholders::_1);
	_dispatch[0x60] = std::bind(&Interpreter::sget, this, std::placeholders::_1);
	_dispatch[0x61] = std::bind(&Interpreter::sget_wide, this, std::placeholders::_1);
	_dispatch[0x62] = std::bind(&Interpreter::sget_object, this, std::placeholders::_1);
	_dispatch[0x63] = std::bind(&Interpreter::sget_boolean, this, std::placeholders::_1);
	_dispatch[0x64] = std::bind(&Interpreter::sget_byte, this, std::placeholders::_1);
	_dispatch[0x65] = std::bind(&Interpreter::sget_char, this, std::placeholders::_1);
	_dispatch[0x66] = std::bind(&Interpreter::sget_short, this, std::placeholders::_1);
	_dispatch[0x67] = std::bind(&Interpreter::sput, this, std::placeholders::_1);
	_dispatch[0x68] = std::bind(&Interpreter::sput_wide, this, std::placeholders::_1);
	_dispatch[0x69] = std::bind(&Interpreter::sput_object, this, std::placeholders::_1);
	_dispatch[0x6A] = std::bind(&Interpreter::sput_boolean, this, std::placeholders::_1);
	_dispatch[0x6B] = std::bind(&Interpreter::sput_byte, this, std::placeholders::_1);
	_dispatch[0x6C] = std::bind(&Interpreter::sput_char, this, std::placeholders::_1);
	_dispatch[0x6D] = std::bind(&Interpreter::sput_short, this, std::placeholders::_1);
	_dispatch[0x6E] = std::bind(&Interpreter::invoke_virtual, this, std::placeholders::_1);
	_dispatch[0x6F] = std::bind(&Interpreter::invoke_super, this, std::placeholders::_1);
	_dispatch[0x70] = std::bind(&Interpreter::invoke_direct, this, std::placeholders::_1);
	_dispatch[0x71] = std::bind(&Interpreter::invoke_static, this, std::placeholders::_1);
	_dispatch[0x72] = std::bind(&Interpreter::invoke_interface, this, std::placeholders::_1);
	// 73 unused
	_dispatch[0x74] = std::bind(&Interpreter::invoke_virtual_range, this, std::placeholders::_1);
	_dispatch[0x75] = std::bind(&Interpreter::invoke_super_range, this, std::placeholders::_1);
	_dispatch[0x76] = std::bind(&Interpreter::invoke_direct_range, this, std::placeholders::_1);
	_dispatch[0x77] = std::bind(&Interpreter::invoke_static_range, this, std::placeholders::_1);
	_dispatch[0x78] = std::bind(&Interpreter::invoke_interface_range, this, std::placeholders::_1);
	// 0x79 ... 0x7A (unused)
	_dispatch[0x7B] = std::bind(&Interpreter::neg_int, this, std::placeholders::_1);
	_dispatch[0x7C] = std::bind(&Interpreter::not_int, this, std::placeholders::_1);
	_dispatch[0x7D] = std::bind(&Interpreter::neg_long, this, std::placeholders::_1);
	_dispatch[0x7E] = std::bind(&Interpreter::not_long, this, std::placeholders::_1);
	_dispatch[0x7F] = std::bind(&Interpreter::neg_float, this, std::placeholders::_1);
	_dispatch[0x80] = std::bind(&Interpreter::neg_double, this, std::placeholders::_1);
	_dispatch[0x81] = std::bind(&Interpreter::int_to_long, this, std::placeholders::_1);
	_dispatch[0x82] = std::bind(&Interpreter::int_to_float, this, std::placeholders::_1);
	_dispatch[0x83] = std::bind(&Interpreter::int_to_double, this, std::placeholders::_1);
	_dispatch[0x84] = std::bind(&Interpreter::long_to_int, this, std::placeholders::_1);
	_dispatch[0x85] = std::bind(&Interpreter::long_to_float, this, std::placeholders::_1);
	_dispatch[0x86] = std::bind(&Interpreter::long_to_double, this, std::placeholders::_1);
	_dispatch[0x87] = std::bind(&Interpreter::float_to_int, this, std::placeholders::_1);
	_dispatch[0x88] = std::bind(&Interpreter::float_to_long, this, std::placeholders::_1);
	_dispatch[0x89] = std::bind(&Interpreter::float_to_double, this, std::placeholders::_1);
	_dispatch[0x8A] = std::bind(&Interpreter::double_to_int, this, std::placeholders::_1);
	_dispatch[0x8B] = std::bind(&Interpreter::double_to_long, this, std::placeholders::_1);
	_dispatch[0x8C] = std::bind(&Interpreter::double_to_float, this, std::placeholders::_1);
	_dispatch[0x8D] = std::bind(&Interpreter::int_to_byte, this, std::placeholders::_1);
	_dispatch[0x8E] = std::bind(&Interpreter::int_to_char, this, std::placeholders::_1);
	_dispatch[0x8F] = std::bind(&Interpreter::int_to_short, this, std::placeholders::_1);
	_dispatch[0x90] = std::bind(&Interpreter::add_int, this, std::placeholders::_1);
	_dispatch[0x91] = std::bind(&Interpreter::sub_int, this, std::placeholders::_1);
	_dispatch[0x92] = std::bind(&Interpreter::mul_int, this, std::placeholders::_1);
	_dispatch[0x93] = std::bind(&Interpreter::div_int, this, std::placeholders::_1);
	_dispatch[0x94] = std::bind(&Interpreter::rem_int, this, std::placeholders::_1);
	_dispatch[0x95] = std::bind(&Interpreter::and_int, this, std::placeholders::_1);
	_dispatch[0x96] = std::bind(&Interpreter::or_int, this, std::placeholders::_1);
	_dispatch[0x97] = std::bind(&Interpreter::xor_int, this, std::placeholders::_1);
	_dispatch[0x98] = std::bind(&Interpreter::shl_int, this, std::placeholders::_1);
	_dispatch[0x99] = std::bind(&Interpreter::shr_int, this, std::placeholders::_1);
	_dispatch[0x9A] = std::bind(&Interpreter::ushr_int, this, std::placeholders::_1);
	_dispatch[0x9B] = std::bind(&Interpreter::add_long, this, std::placeholders::_1);
	_dispatch[0x9C] = std::bind(&Interpreter::sub_long, this, std::placeholders::_1);
	_dispatch[0x9D] = std::bind(&Interpreter::mul_long, this, std::placeholders::_1);
	_dispatch[0x9E] = std::bind(&Interpreter::div_long, this, std::placeholders::_1);
	_dispatch[0x9F] = std::bind(&Interpreter::rem_long, this, std::placeholders::_1);
	_dispatch[0xA0] = std::bind(&Interpreter::and_long, this, std::placeholders::_1);
	_dispatch[0xA1] = std::bind(&Interpreter::or_long, this, std::placeholders::_1);
	_dispatch[0xA2] = std::bind(&Interpreter::xor_long, this, std::placeholders::_1);
	_dispatch[0xA3] = std::bind(&Interpreter::shl_long, this, std::placeholders::_1);
	_dispatch[0xA4] = std::bind(&Interpreter::shr_long, this, std::placeholders::_1);
	_dispatch[0xA5] = std::bind(&Interpreter::ushr_long, this, std::placeholders::_1);
	_dispatch[0xA6] = std::bind(&Interpreter::add_float, this, std::placeholders::_1);
	_dispatch[0xA7] = std::bind(&Interpreter::sub_float, this, std::placeholders::_1);
	_dispatch[0xA8] = std::bind(&Interpreter::mul_float, this, std::placeholders::_1);
	_dispatch[0xA9] = std::bind(&Interpreter::div_float, this, std::placeholders::_1);
	_dispatch[0xAA] = std::bind(&Interpreter::rem_float, this, std::placeholders::_1);
	_dispatch[0xAB] = std::bind(&Interpreter::add_double, this, std::placeholders::_1);
	_dispatch[0xAC] = std::bind(&Interpreter::sub_double, this, std::placeholders::_1);
	_dispatch[0xAD] = std::bind(&Interpreter::mul_double, this, std::placeholders::_1);
	_dispatch[0xAE] = std::bind(&Interpreter::div_double, this, std::placeholders::_1);
	_dispatch[0xAF] = std::bind(&Interpreter::rem_double, this, std::placeholders::_1);
	_dispatch[0xB0] = std::bind(&Interpreter::add_int_2addr, this, std::placeholders::_1);
	_dispatch[0xB1] = std::bind(&Interpreter::sub_int_2addr, this, std::placeholders::_1);
	_dispatch[0xB2] = std::bind(&Interpreter::mul_int_2addr, this, std::placeholders::_1);
	_dispatch[0xB3] = std::bind(&Interpreter::div_int_2addr, this, std::placeholders::_1);
	_dispatch[0xB4] = std::bind(&Interpreter::rem_int_2addr, this, std::placeholders::_1);
	_dispatch[0xB5] = std::bind(&Interpreter::and_int_2addr, this, std::placeholders::_1);
	_dispatch[0xB6] = std::bind(&Interpreter::or_int_2addr, this, std::placeholders::_1);
	_dispatch[0xB7] = std::bind(&Interpreter::xor_int_2addr, this, std::placeholders::_1);
	_dispatch[0xB8] = std::bind(&Interpreter::shl_int_2addr, this, std::placeholders::_1);
	_dispatch[0xB9] = std::bind(&Interpreter::shr_int_2addr, this, std::placeholders::_1);
	_dispatch[0xBA] = std::bind(&Interpreter::ushr_int_2addr, this, std::placeholders::_1);
	_dispatch[0xBB] = std::bind(&Interpreter::add_long_2addr, this, std::placeholders::_1);
	_dispatch[0xBC] = std::bind(&Interpreter::sub_long_2addr, this, std::placeholders::_1);
	_dispatch[0xBD] = std::bind(&Interpreter::mul_long_2addr, this, std::placeholders::_1);
	_dispatch[0xBE] = std::bind(&Interpreter::div_long_2addr, this, std::placeholders::_1);
	_dispatch[0xBF] = std::bind(&Interpreter::rem_long_2addr, this, std::placeholders::_1);
	_dispatch[0xC0] = std::bind(&Interpreter::and_long_2addr, this, std::placeholders::_1);
	_dispatch[0xC1] = std::bind(&Interpreter::or_long_2addr, this, std::placeholders::_1);
	_dispatch[0xC2] = std::bind(&Interpreter::xor_long_2addr, this, std::placeholders::_1);
	_dispatch[0xC3] = std::bind(&Interpreter::shl_long_2addr, this, std::placeholders::_1);
	_dispatch[0xC4] = std::bind(&Interpreter::shr_long_2addr, this, std::placeholders::_1);
	_dispatch[0xC5] = std::bind(&Interpreter::ushr_long_2addr, this, std::placeholders::_1);
	_dispatch[0xC6] = std::bind(&Interpreter::add_float_2addr, this, std::placeholders::_1);
	_dispatch[0xC7] = std::bind(&Interpreter::sub_float_2addr, this, std::placeholders::_1);
	_dispatch[0xC8] = std::bind(&Interpreter::mul_float_2addr, this, std::placeholders::_1);
	_dispatch[0xC9] = std::bind(&Interpreter::div_float_2addr, this, std::placeholders::_1);
	_dispatch[0xCA] = std::bind(&Interpreter::rem_float_2addr, this, std::placeholders::_1);
	_dispatch[0xCB] = std::bind(&Interpreter::add_double_2addr, this, std::placeholders::_1);
	_dispatch[0xCC] = std::bind(&Interpreter::sub_double_2addr, this, std::placeholders::_1);
	_dispatch[0xCD] = std::bind(&Interpreter::mul_double_2addr, this, std::placeholders::_1);
	_dispatch[0xCE] = std::bind(&Interpreter::div_double_2addr, this, std::placeholders::_1);
	_dispatch[0xCF] = std::bind(&Interpreter::rem_double_2addr, this, std::placeholders::_1);
	_dispatch[0xD0] = std::bind(&Interpreter::add_int_lit16, this, std::placeholders::_1);
	_dispatch[0xD1] = std::bind(&Interpreter::rsub_int_lit16, this, std::placeholders::_1);
	_dispatch[0xD2] = std::bind(&Interpreter::mul_int_lit16, this, std::placeholders::_1);
	_dispatch[0xD3] = std::bind(&Interpreter::div_int_lit16, this, std::placeholders::_1);
	_dispatch[0xD4] = std::bind(&Interpreter::rem_int_lit16, this, std::placeholders::_1);
	_dispatch[0xD5] = std::bind(&Interpreter::and_int_lit16, this, std::placeholders::_1);
	_dispatch[0xD6] = std::bind(&Interpreter::or_int_lit16, this, std::placeholders::_1);
	_dispatch[0xD7] = std::bind(&Interpreter::xor_int_lit16, this, std::placeholders::_1);
	_dispatch[0xD8] = std::bind(&Interpreter::add_int_lit8, this, std::placeholders::_1);
	_dispatch[0xD9] = std::bind(&Interpreter::rsub_int_lit8, this, std::placeholders::_1);
	_dispatch[0xDA] = std::bind(&Interpreter::mul_int_lit8, this, std::placeholders::_1);
	_dispatch[0xDB] = std::bind(&Interpreter::div_int_lit8, this, std::placeholders::_1);
	_dispatch[0xDC] = std::bind(&Interpreter::rem_int_lit8, this, std::placeholders::_1);
	_dispatch[0xDD] = std::bind(&Interpreter::and_int_lit8, this, std::placeholders::_1);
	_dispatch[0xDE] = std::bind(&Interpreter::or_int_lit8, this, std::placeholders::_1);
	_dispatch[0xDF] = std::bind(&Interpreter::xor_int_lit8, this, std::placeholders::_1);
	_dispatch[0xE0] = std::bind(&Interpreter::shl_int_lit8, this, std::placeholders::_1);
	_dispatch[0xE1] = std::bind(&Interpreter::shr_int_lit8, this, std::placeholders::_1);
	_dispatch[0xE2] = std::bind(&Interpreter::ushr_int_lit8, this, std::placeholders::_1);
	// 0xE3 ... 0xFF (unused)
}

Interpreter::~Interpreter() {
	/*std::vector<std::pair<uint8_t, uint64_t>> sortedCoverage(_instcoverage.begin(), _instcoverage.end());
	std::sort(sortedCoverage.begin(), sortedCoverage.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
	for (const auto& [key, value] : sortedCoverage) {
	    logger.info(fmt::format("Instruction 0x{:02x} executed {} times", key, value));
	}*/
	auto count = 0;
	for (uint16_t i = 0; i < 0xE3; ++i) {
		if (_instcoverage.find(i) == _instcoverage.end()) {
			auto s = _disassembler->disassemble(i);
			if (!s.empty()) {
				logger.warning(fmt::format("Missing coverage for instruction 0x{:02x} {}", i, s));
				count++;
			}
		}
	}
	logger.info(fmt::format("{} tested, missing coverage for {} instructions", _instcoverage.size(), count - 8));
}

void Interpreter::execute() {
	auto& frame = _rt.currentFrame();
	auto& method = frame.getMethod();
	auto code = method.getBytecode();
	auto func = fmt::format("{}::{}{}", method.getClass().getFullname(), method.getName(), method.getSignature());
	if (code == nullptr) {
		throw std::runtime_error(fmt::format("Method {} has no bytecode!", func));
	}
	if (frame.pc() >= method.getBytecodeSize()) {
		throw std::runtime_error(fmt::format("Current frame {} has invalid pc: {}", func, frame.pc()));
	}
	auto bytecode = code + frame.pc();
	auto inst = _disassembler->disassemble(bytecode);
	logger.info(fmt::format("{:04x}: {:<80} {:<20} ", frame.pc() / 2, inst, func));
	frame.pc()++;
	_instcoverage[*bytecode]++;
	_dispatch[*bytecode](bytecode + 1);
}

void Interpreter::executeClinit(Class& class_) {
	// push new frame with initializeSystemClass method (should be executed after <clinit>)
	try {
		auto& initializeSystemClass = class_.getMethod("initializeSystemClass", "()V");
		_rt.newFrame(initializeSystemClass);
	} catch (std::exception& e) {
		// logger.debug(fmt::format("No initializeSystemClass method for class {}: {}", class_.getFullname(), e.what()));
	}
	// push new frame with <clinit> method
	try {
		auto& clinitMethod = class_.getMethod("<clinit>", "()V");
		if (clinitMethod.isNative()) {
			throw std::runtime_error(fmt::format("Native <clinit> method for class {} is not supported!", class_.getFullname()));
		}
		if (clinitMethod.hasBytecode()) {
			_rt.newFrame(clinitMethod);
		} else {
			std::vector<std::shared_ptr<Object>> args;
			clinitMethod.execute(_rt.currentFrame(), args);
		}
	} catch (std::exception& e) {
		logger.debug(fmt::format("No <clinit> method for class {}: {}", class_.getFullname(), e.what()));
	}
}

void Interpreter::executeNativeMethod(const Method& method_, const std::vector<std::shared_ptr<Object>>& args_) {
	// Construct the JNI symbol name
	std::string symbolName = "Java_" + std::regex_replace(method_.getClass().getFullname(), std::regex("\\."), "_") + "_" + method_.getName();
	if (method_.isOverload()) {
		symbolName += "__" + JNIHelper::mangleMethodSignature(method_.getSignature());
	}
	logger.debug(fmt::format("call native jni function {}", symbolName));
	void* symbol = _rt.vm().findNativeSymbol(symbolName);
	if (!symbol) {
		throw std::runtime_error(fmt::format("Native method {} is not available!", symbolName));
	}

	const auto& desc = method_.getSignature();
	std::regex descriptorRegex(R"(\((.*?)\)(.*))");
	std::smatch match;
	if (!std::regex_match(desc, match, descriptorRegex)) {
		throw std::runtime_error(fmt::format("Invalid method descriptor: {}", desc));
	}
	std::string params = match[1];
	std::string returnType = match[2];
	logger.debug(fmt::format("Executing {}.{}{} -> native function {}@{:#x}", method_.getClass().getFullname(), method_.getName(), method_.getSignature(),
	                         symbolName, (uintptr_t)symbol));
	auto caller = std::make_unique<NativeCallHelper>(*_rt.vm().getJNIEnv());
	auto ret = caller->invoke(symbol, _rt.vm().getJNIEnv(), args_, returnType, params, method_.isStatic());
	_rt.currentFrame().setReturnObject(ret);
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
	uint16_t src = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	frame.setIntRegister(dest, frame.getIntRegister(src));
	frame.pc() += 3;
}
// move/16 vAAAA, vBBBB
void Interpreter::move_16(const uint8_t* operand_) {
	uint16_t dest = *(uint16_t*)&operand_[0];
	uint16_t src = *(uint16_t*)&operand_[1];
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
	uint16_t src = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	frame.setIntRegister(dest, frame.getIntRegister(src));
	frame.setIntRegister(dest + 1, frame.getIntRegister(src + 1));
	frame.pc() += 3;
}
// move-wide/16 vAAAA, vBBBB
void Interpreter::move_wide16(const uint8_t* operand_) {
	uint16_t dest = *(uint16_t*)&operand_[0];
	uint16_t src = *(uint16_t*)&operand_[1];
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
	uint16_t src = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	frame.setObjRegister(dest, frame.getObjRegister(src));
	frame.pc() += 3;
}
// move-object/16 vAAAA, vBBBB
void Interpreter::move_object16(const uint8_t* operand_) {
	uint16_t dest = *(uint16_t*)&operand_[0];
	uint16_t src = *(uint16_t*)&operand_[1];
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
	_rt.currentFrame().setReturnValue(ret);
}
// return-wide vAA
void Interpreter::return_wide(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	auto ret = _rt.currentFrame().getLongRegister(dest);
	_rt.popFrame();
	_rt.currentFrame().setReturnDoubleValue(ret);
}
// return-object vAA
void Interpreter::return_object(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	auto ret = _rt.currentFrame().getObjRegister(dest);
	_rt.popFrame();
	_rt.currentFrame().setReturnObject(ret);
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
	int16_t value = static_cast<int16_t>(*(uint16_t*)&operand_[1]);
	auto& frame = _rt.currentFrame();
	frame.setIntRegister(dest, (int32_t)value);
	frame.pc() += 3;
}
// const vAA, #+BBBBBBBB
void Interpreter::const_(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	int32_t value = *(int32_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	frame.setIntRegister(dest, value);
	frame.pc() += 5;
}
// const/high16 vAA, #+BBBB0000
void Interpreter::const_high16(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint32_t value = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	frame.setIntRegister(dest, value << 16);
	frame.pc() += 3;
}
// const-wide/16 vAA, #+BBBB
void Interpreter::const_wide_16(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	int16_t value = static_cast<int16_t>(*(uint16_t*)&operand_[1]);
	auto& frame = _rt.currentFrame();
	frame.setLongRegister(dest, (int64_t)value);
	frame.pc() += 3;
}
// const-wide/32 vAA, #+BBBBBBBB
void Interpreter::const_wide_32(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	int32_t value = *(int32_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	frame.setLongRegister(dest, value);
	frame.pc() += 5;
}
// const-wide vAA, #+BBBBBBBBBBBBBBBB
void Interpreter::const_wide(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	int64_t value = *(int64_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	frame.setLongRegister(dest, value);
	frame.pc() += 9;
}
// const-wide/high16 vAA, #+BBBB000000000000
void Interpreter::const_wide_high16(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint64_t value = static_cast<uint64_t>(*(uint16_t*)&operand_[1]) << 48;
	auto& frame = _rt.currentFrame();
	frame.setLongRegister(dest, value);
	frame.pc() += 3;
}
// const-string vAA, string@BBBB
void Interpreter::const_string(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t stringIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();
	auto str = classloader.resolveString(frame.getDexIdx(), stringIndex);
	frame.setObjRegister(dest, Object::make(classloader, str));
	frame.pc() += 3;
}
// const-string/jumbo vAA, string@BBBBBBBB
void Interpreter::const_string_jumbo(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint32_t stringIndex = *(uint32_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();
	auto str = classloader.resolveString(frame.getDexIdx(), stringIndex);
	frame.setObjRegister(dest, Object::make(classloader, str));
	frame.pc() += 5;
}
// const-class vAA, type@BBBB
void Interpreter::const_class(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t typeIndex = *(uint16_t*)&operand_[1];
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
	logger.warning(fmt::format("monitor_enter not implemented!"));
	//_rt.monitorEnter(obj);
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
	logger.warning(fmt::format("monitor_exit not implemented!"));
	//_rt.monitorExit(obj);
	frame.pc()++;
}
// check-cast vAA, type@BBBB
void Interpreter::check_cast(const uint8_t* operand_) {
	uint8_t reg = operand_[0];
	uint16_t typeIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto obj = frame.getObjRegister(reg);
	if (obj->isNull()) {
		throw NullPointerException("check_cast on null object");
	}
	auto& classloader = _rt.getClassLoader();
	auto& targetClass = classloader.resolveClass(frame.getDexIdx(), typeIndex);
	if (!targetClass.isInstanceOf(obj)) {
		throw std::runtime_error(fmt::format("Cannot cast object to {}", targetClass.getName()));
	}
	frame.pc() += 3;
}
// instance-of vA, vB, type@CCCC
void Interpreter::instance_of(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	uint16_t typeIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto obj = frame.getObjRegister(src);
	auto& classloader = _rt.getClassLoader();
	auto& targetClass = classloader.resolveClass(frame.getDexIdx(), typeIndex);
	frame.setIntRegister(dest, !obj->isNull() && targetClass.isInstanceOf(obj) ? 1 : 0);
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
	uint16_t typeIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();
	auto& cls = classloader.resolveClass(frame.getDexIdx(), typeIndex);

	if (cls.isAbstract() || cls.isInterface()) {
		throw std::runtime_error(fmt::format("Cannot instantiate abstract class or interface: {}", cls.getName()));
	}

	if (!cls.isStaticInitialized()) {
		frame.pc()--;
		executeClinit(cls);
		return;
	}

	logger.debug(fmt::format("new {}", cls.getFullname()));
	frame.setObjRegister(dest, Object::make(cls));
	frame.pc() += 3;
}
// new-array vA, vB, type@CCCC
void Interpreter::new_array(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t src = (operand_[0] >> 4) & 0x0F;
	uint16_t typeIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();

	auto& classloader = _rt.getClassLoader();
	auto arrayType = classloader.resolveArray(frame.getDexIdx(), typeIndex);

	// Get the array size from the source register
	int32_t size = frame.getIntRegister(src);
	if (size < 0) {
		throw std::runtime_error("new_array: Array size cannot be negative");
	}
	auto arrayObj = Array::make(arrayType[0].first, size);
	frame.setObjRegister(dest, arrayObj);
	frame.pc() += 3;
}
// filled-new-array {vD, vE, vF, vG, vA}, type@CCCC
void Interpreter::filled_new_array(const uint8_t* operand_) {
	throw std::runtime_error("filled_new_array not implemented");
}
// filled-new-array/range {vCCCC .. vNNNN}, type@BBBB
void Interpreter::filled_new_array_range(const uint8_t* operand_) {
	throw std::runtime_error("filled_new_array_range not implemented");
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
		throw std::runtime_error(fmt::format("Invalid array-data identifier: 0x{:04x}", ident));
	}

	uint32_t elementSize = data[1];
	uint32_t elementCount = *reinterpret_cast<const uint32_t*>(&data[2]);
	auto arrayData = reinterpret_cast<const uint8_t*>(&data[4]);

	auto arrayObj = frame.getObjRegister(reg);
	if (arrayObj->isNull()) {
		throw NullPointerException("fill_array_data on null array object");
	}

	auto array = std::dynamic_pointer_cast<Array>(arrayObj);
	if (!array) {
		throw std::runtime_error("fill_array_data: Object is not an array");
	}

	if (array->getArrayLength() != elementCount) {
		throw std::runtime_error("fill_array_data: Array length mismatch");
	}

	for (uint32_t i = 0; i < elementCount; ++i) {
		switch (elementSize) {
			case 1: {
				int8_t value = *reinterpret_cast<const int8_t*>(arrayData);
				array->setArrayElement(i, Object::make(value));
				arrayData += 1;
				break;
			}
			case 2: {
				int16_t value = *reinterpret_cast<const int16_t*>(arrayData);
				array->setArrayElement(i, Object::make(value));
				arrayData += 2;
				break;
			}
			case 4: {
				int32_t value = *reinterpret_cast<const int32_t*>(arrayData);
				array->setArrayElement(i, Object::make(value));
				arrayData += 4;
				break;
			}
			case 8: {
				int64_t value = *reinterpret_cast<const int64_t*>(arrayData);
				array->setArrayElement(i, Object::make(value));
				arrayData += 8;
				break;
			}
			default:
				throw std::runtime_error(fmt::format("fill_array_data: Unsupported element size: {}", elementSize));
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

	while (1) {
		try {
			auto& frame = _rt.currentFrame();
			auto& method = frame.getMethod();
			uint32_t catchAllAddrress = 0;
			auto exceptionHandler = method.getExceptionHandler(frame.pc() - 1, catchAllAddrress);
			for (auto& exc : exceptionHandler) {
				auto& classloader = _rt.getClassLoader();
				auto& exceptionType = classloader.resolveClass(frame.getDexIdx(), exc.first);
				if (exceptionType.isInstanceOf(obj)) {
					logger.debug(fmt::format("Catch exception {} at {:x}", exceptionType.getName(), exc.second));
					frame.pc() = exc.second << 1;
					frame.setException(obj);
					return;
				}
			}
			if (catchAllAddrress != 0) {
				logger.debug(fmt::format("Catch all exception at {:x}", catchAllAddrress));
				frame.pc() = catchAllAddrress << 1;
				frame.setException(obj);
				return;
			}
			// No handler found - propagate to caller
			_rt.popFrame();
			_rt.currentFrame().setException(obj);
		} catch (const std::exception& e) {
			try {
				_rt.popFrame();
				_rt.currentFrame().setException(obj);
				continue;
			} catch (const std::exception& e) {
				throw std::runtime_error(fmt::format("Unhandled exception {}", obj->debug()));
			}
		}
	}
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
		throw std::runtime_error(fmt::format("Invalid packed-switch identifier: 0x{:04x}", ident));
	}
	int32_t size = switchData[1];
	if (size <= 0) {
		throw std::runtime_error("packed-switch: Invalid size in switch data");
	}

	auto base = *reinterpret_cast<const int32_t*>(&switchData[2]);
	auto targets = reinterpret_cast<const int32_t*>(&switchData[4]);
	if (!targets) {
		throw std::runtime_error("packed-switch: Invalid targets pointer");
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
		throw std::runtime_error(fmt::format("Invalid sparse-switch identifier: 0x{:04x}", ident));
	}
	int32_t size = switchData[1];
	if (size <= 0) {
		throw std::runtime_error("sparse-switch: Invalid size in switch data");
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
		throw NullPointerException("if-ne on null object");
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
		throw NullPointerException("if-ne on null object");
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
	if (frame.getIntRegister(regA) == 0) {
		frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
	} else {
		frame.pc() += 3;
	}
}
// if-nez vAA, +BBBB
void Interpreter::if_nez(const uint8_t* operand_) {
	uint8_t regA = operand_[0];
	int16_t offset = *reinterpret_cast<const int16_t*>(&operand_[1]);
	auto& frame = _rt.currentFrame();
	if (frame.getIntRegister(regA) != 0) {
		frame.pc() += (offset << 1) - 1;  // -1 because pc is incremented before.
	} else {
		frame.pc() += 3;
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
		throw std::runtime_error("aget: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw std::runtime_error("aget: Array index out of bounds");
	}
	auto obj = array->getArrayElement(index);
	if (!obj->isNumberObject()) {
		throw std::runtime_error("aget: Array does not contain number");
	}
	auto numberObj = std::dynamic_pointer_cast<NumberObject>(obj);
	if (!numberObj) {
		throw std::runtime_error("aget: Array element is not a number object");
	}
	frame.setIntRegister(dest, numberObj->getValue());
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
		throw std::runtime_error("aget-wide: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw std::runtime_error("aget-wide: Array index out of bounds");
	}
	auto numberObj = std::dynamic_pointer_cast<NumberObject>(array->getArrayElement(index));
	if (!numberObj) {
		throw std::runtime_error("aget-wide: Array element is not a number object");
	}
	int64_t value = numberObj->getLongValue();
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
		throw std::runtime_error("aget-object: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw std::runtime_error("aget-object: Array index out of bounds");
	}
	auto value = array->getArrayElement(index);
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
		throw std::runtime_error("aget-boolean: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw std::runtime_error("aget-boolean: Array index out of bounds");
	}
	auto element = array->getArrayElement(index);
	auto numberObj = std::dynamic_pointer_cast<NumberObject>(element);
	if (!numberObj) {
		throw std::runtime_error("aget-boolean: Array element is not a number object");
	}
	bool value = numberObj->getValue() != 0;
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
		throw std::runtime_error("aget-byte: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw std::runtime_error("aget-byte: Array index out of bounds");
	}
	auto numberObj = std::dynamic_pointer_cast<NumberObject>(array->getArrayElement(index));
	if (!numberObj) {
		throw std::runtime_error("aget-byte: Array element is not a number object");
	}
	int8_t value = static_cast<int8_t>(numberObj->getValue());
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
		throw std::runtime_error("aget-char: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw std::runtime_error("aget-char: Array index out of bounds");
	}
	auto numberObj = std::dynamic_pointer_cast<NumberObject>(array->getArrayElement(index));
	if (!numberObj) {
		throw std::runtime_error("aget-char: Array element is not a number object");
	}
	uint16_t value = static_cast<uint16_t>(numberObj->getValue());
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
		throw std::runtime_error("aget-short: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw std::runtime_error("aget-short: Array index out of bounds");
	}
	auto numberObj = std::dynamic_pointer_cast<NumberObject>(array->getArrayElement(index));
	if (!numberObj) {
		throw std::runtime_error("aget-short: Array element is not a number object");
	}
	int16_t value = static_cast<int16_t>(numberObj->getValue());
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// aput vAA, vBB, vCC
void Interpreter::aput(const uint8_t* operand_) {
	uint8_t valueReg = operand_[0];
	uint8_t arrayReg = operand_[1];
	uint8_t indexReg = operand_[2];
	auto& frame = _rt.currentFrame();

	logger.debug(fmt::format("aput: valueReg: {}, arrayReg: {}, indexReg: {}", valueReg, arrayReg, indexReg));

	auto arrayObj = frame.getObjRegister(arrayReg);
	if (arrayObj->isNull()) {
		throw NullPointerException("aput on null array object");
	}
	auto array = std::dynamic_pointer_cast<Array>(arrayObj);
	if (!array) {
		throw std::runtime_error("aput: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	int32_t value = frame.getIntRegister(valueReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw std::runtime_error("aput: Array index out of bounds");
	}
	array->setArrayElement(index, Object::make(value));
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
		throw std::runtime_error("aput-wide: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw std::runtime_error("aput-wide: Array index out of bounds");
	}
	auto value = frame.getLongRegister(valueReg);
	array->setArrayElement(index, Object::make(value));
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
		throw std::runtime_error("aput-object: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	auto value = frame.getObjRegister(valueReg);
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw std::runtime_error("aput-object: Array index out of bounds");
	}
	array->setArrayElement(index, value);
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
		throw std::runtime_error("aput-boolean: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	bool value = frame.getIntRegister(valueReg) != 0;
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw std::runtime_error("aput-boolean: Array index out of bounds");
	}
	array->setArrayElement(index, Object::make(value));
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
		throw std::runtime_error("aput-byte: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	int8_t value = static_cast<int8_t>(frame.getIntRegister(valueReg));
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw std::runtime_error("aput-byte: Array index out of bounds");
	}
	array->setArrayElement(index, Object::make(value));
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
		throw std::runtime_error("aput-char: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	uint16_t value = static_cast<uint16_t>(frame.getIntRegister(valueReg));
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw std::runtime_error("aput-char: Array index out of bounds");
	}
	array->setArrayElement(index, Object::make(value));
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
		throw std::runtime_error("aput-short: Object is not an array");
	}

	int32_t index = frame.getIntRegister(indexReg);
	int16_t value = static_cast<int16_t>(frame.getIntRegister(valueReg));
	if (index < 0 || (uint32_t)index >= array->getArrayLength()) {
		throw std::runtime_error("aput-short: Array index out of bounds");
	}
	array->setArrayElement(index, Object::make(value));
	frame.pc() += 3;
}
// iget vA, vB, field@CCCC
void Interpreter::iget(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iget on null object");
	}

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "I" && field.getType() != "F") {
		throw std::runtime_error(fmt::format("iget: Field {} type mismatch, expected int but got {}", field.getName(), field.getType()));
	}

	auto fieldObj = obj->getField(field.getName());
	if (!fieldObj || !fieldObj->isNumberObject()) {
		throw std::runtime_error(fmt::format("iget: Field {} is not a number object", field.getName()));
	}
	int32_t value = static_cast<int32_t>(std::dynamic_pointer_cast<NumberObject>(fieldObj)->getValue());
	logger.debug(fmt::format("iget {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// iget-wide vA, vB, field@CCCC
void Interpreter::iget_wide(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iget_wide on null object");
	}

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "J" && field.getType() != "D") {
		throw std::runtime_error(fmt::format("iget_wide: Field {} type mismatch, expected long or double but got {}", field.getName(), field.getType()));
	}
	auto fieldObj = obj->getField(field.getName());
	if (!fieldObj || !fieldObj->isNumberObject()) {
		throw std::runtime_error(fmt::format("iget_wide: Field {} is not a number object", field.getName()));
	}

	auto value = std::dynamic_pointer_cast<NumberObject>(fieldObj)->getLongValue();
	logger.debug(fmt::format("iget_wide {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	frame.setLongRegister(dest, value);
	frame.pc() += 3;
}
// iget-object vA, vB, field@CCCC
void Interpreter::iget_object(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iget_object on null object");
	}

	// class should have been loaded if trying to access an instance field. resolveField should not fail.
	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType()[0] != 'L' && field.getType()[0] != '[') {
		throw std::runtime_error(fmt::format("iget_object: Field {} type mismatch, expected object or array but got {}", field.getName(), field.getType()));
	}

	auto fieldObj = obj->getField(field.getName());
	logger.debug(fmt::format("iget_object {}.{}={}", field.getClass().getFullname(), field.getName(), fieldObj ? fieldObj->debug() : "null"));
	frame.setObjRegister(dest, fieldObj);
	frame.pc() += 3;
}
// iget-boolean vA, vB, field@CCCC
void Interpreter::iget_boolean(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iget_boolean on null object");
	}

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "Z") {
		throw std::runtime_error(fmt::format("iget_boolean: Field {} type mismatch, expected boolean but got {}", field.getName(), field.getType()));
	}

	auto fieldObj = obj->getField(field.getName());
	if (!fieldObj || !fieldObj->isNumberObject()) {
		throw std::runtime_error(fmt::format("iget_boolean: Field {} is not a number object", field.getName()));
	}
	bool value = static_cast<bool>(std::dynamic_pointer_cast<NumberObject>(fieldObj)->getValue());
	logger.debug(fmt::format("iget_boolean {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// iget-byte vA, vB, field@CCCC
void Interpreter::iget_byte(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iget_byte on null object");
	}

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "B") {
		throw std::runtime_error(fmt::format("iget_byte: Field {} type mismatch, expected byte but got {}", field.getName(), field.getType()));
	}

	auto fieldObj = obj->getField(field.getName());
	if (!fieldObj || !fieldObj->isNumberObject()) {
		throw std::runtime_error(fmt::format("iget_byte: Field {} is not a number object", field.getName()));
	}
	int8_t value = static_cast<int8_t>(std::dynamic_pointer_cast<NumberObject>(fieldObj)->getValue());
	logger.debug(fmt::format("iget_byte {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// iget-char vA, vB, field@CCCC
void Interpreter::iget_char(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iget_char on null object");
	}

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "C") {
		throw std::runtime_error(fmt::format("iget_char: Field {} type mismatch, expected char but got {}", field.getName(), field.getType()));
	}

	auto fieldObj = obj->getField(field.getName());
	if (!fieldObj || !fieldObj->isNumberObject()) {
		throw std::runtime_error(fmt::format("iget_byte: Field {} is not a number object", field.getName()));
	}
	uint16_t value = static_cast<uint16_t>(std::dynamic_pointer_cast<NumberObject>(fieldObj)->getValue());
	logger.debug(fmt::format("iget_char {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// iget-short vA, vB, field@CCCC
void Interpreter::iget_short(const uint8_t* operand_) {
	uint8_t dest = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iget_short on null object");
	}

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "S") {
		throw std::runtime_error(fmt::format("iget_short: Field {} type mismatch, expected short but got {}", field.getName(), field.getType()));
	}

	auto fieldObj = obj->getField(field.getName());
	if (!fieldObj || !fieldObj->isNumberObject()) {
		throw std::runtime_error(fmt::format("iget_byte: Field {} is not a number object", field.getName()));
	}
	int16_t value = static_cast<int16_t>(std::dynamic_pointer_cast<NumberObject>(fieldObj)->getValue());
	logger.debug(fmt::format("iget_short {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// iput vA, vB, field@CCCC
void Interpreter::iput(const uint8_t* operand_) {
	uint8_t src = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iput on null object");
	}

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "I" && field.getType() != "F") {
		throw std::runtime_error(fmt::format("iput: Field {} type mismatch, expected int but got {}", field.getName(), field.getType()));
	}
	int32_t value = frame.getIntRegister(src);
	logger.debug(fmt::format("iput {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	obj->setField(field.getName(), Object::make(value));
	frame.pc() += 3;
}
// iput-wide vA, vB, field@CCCC
void Interpreter::iput_wide(const uint8_t* operand_) {
	uint8_t src = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iput_wide on null object");
	}

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.isStatic()) {
		throw std::runtime_error("iput_wide: Cannot use iput_wide on a static field");
	}

	if (field.getType() != "J" && field.getType() != "D") {
		throw std::runtime_error(fmt::format("iput_wide: Field {} type mismatch, expected long or double but got {}", field.getName(), field.getType()));
	}

	int64_t value = frame.getLongRegister(src);
	logger.debug(fmt::format("iput_wide {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	obj->setField(field.getName(), Object::make(value));
	frame.pc() += 3;
}
// iput-object vA, vB, field@CCCC
void Interpreter::iput_object(const uint8_t* operand_) {
	uint8_t src = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iput_object on null object");
	}

	// class should have been loaded if trying to access an instance field. resolveField should not fail.
	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.isStatic()) {
		throw std::runtime_error("iput_object: Cannot use iput_object on a static field");
	}

	if (field.getType()[0] != 'L' && field.getType()[0] != '[') {
		throw std::runtime_error(fmt::format("iput_object: Field {} type mismatch, expected object or array but got {}", field.getName(), field.getType()));
	}

	auto value = frame.getObjRegister(src);
	logger.debug(fmt::format("iput_object {}.{}={}", field.getClass().getFullname(), field.getName(), value->debug()));
	obj->setField(field.getName(), value);
	frame.pc() += 3;
}
// iput-boolean vA, vB, field@CCCC
void Interpreter::iput_boolean(const uint8_t* operand_) {
	uint8_t src = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iput_boolean on null object");
	}

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "Z") {
		throw std::runtime_error(fmt::format("iput_boolean: Field {} type mismatch, expected boolean but got {}", field.getName(), field.getType()));
	}

	bool value = frame.getIntRegister(src) != 0;
	logger.debug(fmt::format("iput_boolean {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	obj->setField(field.getName(), Object::make(value));
	frame.pc() += 3;
}
// iput-byte vA, vB, field@CCCC
void Interpreter::iput_byte(const uint8_t* operand_) {
	uint8_t src = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iput_byte on null object");
	}

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "B") {
		throw std::runtime_error(fmt::format("iput_byte: Field {} type mismatch, expected byte but got {}", field.getName(), field.getType()));
	}

	int8_t value = static_cast<int8_t>(frame.getIntRegister(src));
	logger.debug(fmt::format("iput_byte {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	obj->setField(field.getName(), Object::make(value));
	frame.pc() += 3;
}
// iput-char vA, vB, field@CCCC
void Interpreter::iput_char(const uint8_t* operand_) {
	uint8_t src = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iput_char on null object");
	}

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "C") {
		throw std::runtime_error(fmt::format("iput_char: Field {} type mismatch, expected char but got {}", field.getName(), field.getType()));
	}

	uint16_t value = static_cast<uint16_t>(frame.getIntRegister(src));
	logger.debug(fmt::format("iput_char {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	obj->setField(field.getName(), Object::make(value));
	frame.pc() += 3;
}
// iput-short vA, vB, field@CCCC
void Interpreter::iput_short(const uint8_t* operand_) {
	uint8_t src = operand_[0] & 0x0F;
	uint8_t objReg = (operand_[0] >> 4) & 0x0F;
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto obj = frame.getObjRegister(objReg);
	if (obj->isNull()) {
		throw NullPointerException("iput_short on null object");
	}

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "S") {
		throw std::runtime_error(fmt::format("iput_short: Field {} type mismatch, expected short but got {}", field.getName(), field.getType()));
	}

	int16_t value = static_cast<int16_t>(frame.getIntRegister(src));
	logger.debug(fmt::format("iput_short {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	obj->setField(field.getName(), Object::make(value));
	frame.pc() += 3;
}
// sget vA, field@BBBB
void Interpreter::sget(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "I" && field.getType() != "F") {
		throw std::runtime_error(fmt::format("sget: Field {} type mismatch, expected int but got {}", field.getName(), field.getType()));
	}

	int32_t value = field.getIntValue();
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// sget-wide vA, field@BBBB
void Interpreter::sget_wide(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "J" && field.getType() != "D") {
		throw std::runtime_error(fmt::format("sget_wide: Field {} type mismatch, expected long or double but got {}", field.getName(), field.getType()));
	}

	int64_t value = field.getLongValue();
	frame.setLongRegister(dest, value);
	frame.pc() += 3;
}
// sget-object vA, field@BBBB
void Interpreter::sget_object(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	std::string classname, fieldname;
	try {
		auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex, classname, fieldname);
		if (!field.isStatic()) {
			throw std::runtime_error("sget_object: Cannot use sget_object on a non-static field");
		}
		if (field.getType()[0] != 'L' && field.getType()[0] != '[') {
			throw std::runtime_error(fmt::format("sget_object: Field {} type mismatch, expected object but got {}", field.getName(), field.getType()));
		}
		// static field access, class instance may not be instantiated yet
		auto& clazz = field.getClass();
		if (!clazz.isStaticInitialized()) {
			// cancel the current instruction
			frame.pc()--;
			executeClinit(clazz);
			return;
		}
		// set result of the sget-object to the destination register
		frame.setObjRegister(dest, field.getObjectValue());
	} catch (const std::exception& e) {
		// if (_rt.handleClassFieldGetter(classname, fieldname)) {
		//	// handle by vm
		//	logger.warning(fmt::format("sget_object handle static field getter by vm for {}->{}!", classname, fieldname));
		//	frame.setObjRegister(dest, Object::makeVmObject(classname + "." + fieldname));
		// } else {
		throw std::runtime_error(fmt::format("sget_object: Failed to resolve field {}.{}: {}", classname, fieldname, e.what()));
		//}
	}
	frame.pc() += 3;
}
// sget-boolean vA, field@BBBB
void Interpreter::sget_boolean(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "Z") {
		throw std::runtime_error(fmt::format("sget_boolean: Field {} type mismatch, expected boolean but got {}", field.getName(), field.getType()));
	}

	bool value = field.getIntValue() != 0;
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// sget-byte vA, field@BBBB
void Interpreter::sget_byte(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "B") {
		throw std::runtime_error(fmt::format("sget_byte: Field {} type mismatch, expected byte but got {}", field.getName(), field.getType()));
	}

	int8_t value = static_cast<int8_t>(field.getIntValue());
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// sget-char vA, field@BBBB
void Interpreter::sget_char(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "C") {
		throw std::runtime_error(fmt::format("sget_char: Field {} type mismatch, expected char but got {}", field.getName(), field.getType()));
	}

	uint16_t value = static_cast<uint16_t>(field.getIntValue());
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// sget-short vA, field@BBBB
void Interpreter::sget_short(const uint8_t* operand_) {
	uint8_t dest = operand_[0];
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (field.getType() != "S") {
		throw std::runtime_error(fmt::format("sget_short: Field {} type mismatch, expected short but got {}", field.getName(), field.getType()));
	}

	int16_t value = static_cast<int16_t>(field.getIntValue());
	frame.setIntRegister(dest, value);
	frame.pc() += 3;
}
// sput vA, field@BBBB
void Interpreter::sput(const uint8_t* operand_) {
	uint8_t src = operand_[0];
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw std::runtime_error("sput: Cannot use sput on a non-static field");
	}

	if (field.getType() != "I" && field.getType() != "F") {
		throw std::runtime_error(fmt::format("sput: Field {} type mismatch, expected int but got {}", field.getName(), field.getType()));
	}

	int32_t value = frame.getIntRegister(src);
	logger.debug(fmt::format("sput {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	field.setIntValue(value);
	frame.pc() += 3;
}
// sput-wide vA, field@BBBB
void Interpreter::sput_wide(const uint8_t* operand_) {
	uint8_t src = operand_[0];
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw std::runtime_error("sput_wide: Cannot use sput_wide on a non-static field");
	}

	if (field.getType() != "J" && field.getType() != "D") {
		throw std::runtime_error(fmt::format("sput_wide: Field {} type mismatch, expected long or double but got {}", field.getName(), field.getType()));
	}

	int64_t value = frame.getLongRegister(src);
	logger.debug(fmt::format("sput_wide {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	field.setLongValue(value);
	frame.pc() += 3;
}
// sput-object vA, field@BBBB
void Interpreter::sput_object(const uint8_t* operand_) {
	uint8_t src = operand_[0];
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	std::string classname, fieldname;
	try {
		auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex, classname, fieldname);
		if (!field.isStatic()) {
			throw std::runtime_error("sput_object: Cannot use sput_object on a non-static field");
		}
		if (field.getType()[0] != 'L' && field.getType()[0] != '[') {
			throw std::runtime_error(fmt::format("sput_object: Field {} type mismatch, expected object but got {}", field.getName(), field.getType()));
		}
		// static field access, class instance may not be instantiated yet
		auto& clazz = field.getClass();
		if (!clazz.isStaticInitialized()) {
			// cancel the current instruction
			frame.pc()--;
			executeClinit(clazz);
			return;
		}
		auto& fieldclass = classloader.getOrLoad(field.getPrettyType());
		// set result of the sput-object
		auto value = frame.getObjRegister(src);
		logger.debug(fmt::format("sput_object {}.{}={}", field.getClass().getFullname(), field.getName(), value->debug()));
		// @todo : this is a hack to handle setting null to an object field. Need to properly handle this case.
		if (value->isNumberObject() && (std::dynamic_pointer_cast<NumberObject>(value)->getValue() == 0)) {
			auto obj = Object::make(fieldclass);
			field.setObjectValue(obj);
		} else {
			field.setObjectValue(value);
		}
	} catch (const std::exception& e) {
		throw std::runtime_error(fmt::format("sput_object: Failed to resolve field {}.{}: {}", classname, fieldname, e.what()));
	}
	frame.pc() += 3;
}
// sput-boolean vA, field@BBBB
void Interpreter::sput_boolean(const uint8_t* operand_) {
	uint8_t src = operand_[0];
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw std::runtime_error("sput_boolean: Cannot use sput_boolean on a non-static field");
	}

	if (field.getType() != "Z") {
		throw std::runtime_error(fmt::format("sput_boolean: Field {} type mismatch, expected boolean but got {}", field.getName(), field.getType()));
	}

	bool value = frame.getIntRegister(src) != 0;
	logger.debug(fmt::format("sput_boolean {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	field.setIntValue(value);
	frame.pc() += 3;
}
// sput-byte vA, field@BBBB
void Interpreter::sput_byte(const uint8_t* operand_) {
	uint8_t src = operand_[0];
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw std::runtime_error("sput_byte: Cannot use sput_byte on a non-static field");
	}

	if (field.getType() != "B") {
		throw std::runtime_error(fmt::format("sput_byte: Field {} type mismatch, expected byte but got {}", field.getName(), field.getType()));
	}

	int8_t value = static_cast<int8_t>(frame.getIntRegister(src));
	logger.debug(fmt::format("sput_byte {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	field.setIntValue(value);
	frame.pc() += 3;
}
// sput-char vA, field@BBBB
void Interpreter::sput_char(const uint8_t* operand_) {
	uint8_t src = operand_[0];
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw std::runtime_error("sput_char: Cannot use sput_char on a non-static field");
	}

	if (field.getType() != "C") {
		throw std::runtime_error(fmt::format("sput_char: Field {} type mismatch, expected char but got {}", field.getName(), field.getType()));
	}

	uint16_t value = static_cast<uint16_t>(frame.getIntRegister(src));
	logger.debug(fmt::format("sput_char {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	field.setIntValue(value);
	frame.pc() += 3;
}
// sput-short vA, field@BBBB
void Interpreter::sput_short(const uint8_t* operand_) {
	uint8_t src = operand_[0];
	uint16_t fieldIndex = *(uint16_t*)&operand_[1];
	auto& frame = _rt.currentFrame();
	auto& classloader = _rt.getClassLoader();

	auto& field = classloader.resolveField(frame.getDexIdx(), fieldIndex);
	if (!field.isStatic()) {
		throw std::runtime_error("sput_short: Cannot use sput_short on a non-static field");
	}

	if (field.getType() != "S") {
		throw std::runtime_error(fmt::format("sput_short: Field {} type mismatch, expected short but got {}", field.getName(), field.getType()));
	}

	int16_t value = static_cast<int16_t>(frame.getIntRegister(src));
	logger.debug(fmt::format("sput_short {}.{}={}", field.getClass().getFullname(), field.getName(), value));
	field.setIntValue(value);
	frame.pc() += 3;
}
// invoke-virtual {vD, vE, vF, vG, vA}, meth@CCCC
void Interpreter::invoke_virtual(const uint8_t* operand_) {
	const uint8_t vA = (operand_[0] >> 4) & 0x0F;  // Number of registers (A)
	auto& classloader = _rt.getClassLoader();
	auto& frame = _rt.currentFrame();

	uint16_t methodRef = *(uint16_t*)&operand_[1];
	uint8_t vC = (vA > 0) ? operand_[3] & 0x0F : 0;
	uint8_t vD = (vA > 0) ? (operand_[3] >> 4) & 0x0F : 0;
	uint8_t vE = (vA > 1) ? operand_[4] & 0x0F : 0;
	uint8_t vF = (vA > 1) ? (operand_[4] >> 4) & 0x0F : 0;
	uint8_t vG = (vA > 2) ? operand_[0] & 0x0F : 0;

	std::vector<uint8_t> regs = {vC, vD, vE, vF, vG};

	std::string args_str = "(";
	std::vector<std::shared_ptr<Object>> args{};
	for (uint8_t i = 0; i < vA; ++i) {
		auto obj = frame.getObjRegister(regs[i]);
		if (i == 0) {
			args_str += "this=";
		}
		args_str += obj->debug();
		if (i < vA - 1) {
			args_str += ", ";
		}
		args.push_back(obj);
	}
	args_str += ")";

	auto this_ptr = frame.getObjRegister(regs[0]);
	if (this_ptr->isNull()) {
		throw NullPointerException("invoke-virtual on null object");
	}
	auto this_ptr_class = std::dynamic_pointer_cast<ObjectClass>(this_ptr);
	if (this_ptr_class == nullptr) {
		throw std::runtime_error(fmt::format("invoke-virtual: this pointer is not an ObjectClass, got {}", this_ptr->debug()));
	}
	Class* instance = &this_ptr_class->getClass();
	if (!instance->isStaticInitialized()) {
		throw std::runtime_error(fmt::format("invoke-virtual: class {} is not static initialized", instance->getFullname()));
	}

	std::string classname, methodname, signature;
	classloader.findMethod(frame.getDexIdx(), methodRef, classname, methodname, signature);

	Method* vmethod = nullptr;
	while (1) {
		try {
			vmethod = &instance->getMethod(methodname, signature);
			break;  // Method found, exit loop
		} catch (std::exception& e) {
			logger.debug(fmt::format("invoke-virtual: method {}->{}{} not found, trying superclass", instance->getFullname(), methodname, signature));
			if (instance->hasSuperClass()) {
				// If the method is not found in the current class, try the superclass
				instance = &classloader.getOrLoad(instance->getSuperClassname());
				if (!instance->isStaticInitialized()) {
					frame.pc()--;
					executeClinit(*instance);
					return;
				}
			} else {
				// If no superclass, break the loop
				break;
			}
		}
	}
	if (vmethod) {
		if (!vmethod->isVirtual()) {
			logger.error(fmt::format("invoke-virtual: method {}->{}{} is not virtual", this_ptr_class->getClass().getFullname(), methodname, signature));
		}
		if (vmethod->isNative()) {
			executeNativeMethod(*vmethod, args);
		} else {
			logger.ok(fmt::format("invoke-virtual call method {}->{}{}{} on instance {}", instance->getFullname(), methodname, signature, args_str,
			                      this_ptr_class->getClass().getFullname()));
			if (vmethod->hasBytecode()) {
				auto& newframe = _rt.newFrame(*vmethod);
				// When a method is invoked, the parameters to the method are placed into the last n registers.
				for (uint32_t i = 0; i < vA; i++) {
					newframe.setObjRegister(vmethod->getNbRegisters() - vA + i, frame.getObjRegister(regs[i]));
				}
			} else {
				vmethod->execute(frame, args);
			}
		}
	} else {
		// If no method found, throw an error
		throw std::runtime_error(
		    fmt::format("invoke-virtual: call method {}->{}{}{} not found", this_ptr_class->getClass().getFullname(), methodname, signature, args_str));
	}

	frame.pc() += 5;
}
// invoke-super {vD, vE, vF, vG, vA}, meth@CCCC
void Interpreter::invoke_super(const uint8_t* operand_) {
	invoke_direct(operand_);
}
// invoke-direct {vD, vE, vF, vG, vA}, meth@CCCC
void Interpreter::invoke_direct(const uint8_t* operand_) {
	const uint8_t vA = (operand_[0] >> 4) & 0x0F;  // Number of registers (A)
	auto& classloader = _rt.getClassLoader();
	auto& frame = _rt.currentFrame();

	uint16_t methodRef = *(uint16_t*)&operand_[1];
	uint8_t vC = (vA > 0) ? operand_[3] & 0x0F : 0;
	uint8_t vD = (vA > 0) ? (operand_[3] >> 4) & 0x0F : 0;
	uint8_t vE = (vA > 1) ? operand_[4] & 0x0F : 0;
	uint8_t vF = (vA > 1) ? (operand_[4] >> 4) & 0x0F : 0;
	uint8_t vG = (vA > 2) ? operand_[0] & 0x0F : 0;

	std::vector<uint8_t> regs = {vC, vD, vE, vF, vG};
	auto& method = classloader.resolveMethod(frame.getDexIdx(), methodRef);
	auto& cls = method.getClass();
	if (!cls.isStaticInitialized()) {
		frame.pc()--;
		executeClinit(cls);
		return;
	}
	auto method_str = fmt::format("{}.{}{}(", method.getClass().getFullname(), method.getName(), method.getSignature());
	std::vector<std::shared_ptr<Object>> args{};
	for (uint8_t i = 0; i < vA; ++i) {
		auto obj = frame.getObjRegister(regs[i]);
		if (i == 0 && !method.isStatic()) {
			method_str += "this=";
		}
		method_str += obj->debug();
		if (i < vA - 1) {
			method_str += ", ";
		}
		args.push_back(obj);
	}
	method_str += ")";

	if (method.isNative()) {
		executeNativeMethod(method, args);
	} else {
		if (method.getBytecode() == nullptr) {
			method.execute(frame, args);
		} else {
			logger.ok(fmt::format("invoke-direct call method {} static={}", method_str, method.isStatic()));
			auto& newframe = _rt.newFrame(method);
			// set args on new frame
			// When a method is invoked, the parameters to the method are placed into the last n registers.
			for (uint32_t i = 0; i < vA; i++) {
				newframe.setObjRegister(method.getNbRegisters() - vA + i, frame.getObjRegister(regs[i]));
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
	const uint8_t vA = (operand_[0] >> 4) & 0x0F;  // Number of registers (A)
	auto& classloader = _rt.getClassLoader();
	auto& frame = _rt.currentFrame();

	uint16_t methodRef = *(uint16_t*)&operand_[1];
	uint8_t vC = (vA > 0) ? operand_[3] & 0x0F : 0;
	uint8_t vD = (vA > 0) ? (operand_[3] >> 4) & 0x0F : 0;
	uint8_t vE = (vA > 1) ? operand_[4] & 0x0F : 0;
	uint8_t vF = (vA > 1) ? (operand_[4] >> 4) & 0x0F : 0;
	uint8_t vG = (vA > 2) ? operand_[0] & 0x0F : 0;

	std::vector<uint8_t> regs = {vC, vD, vE, vF, vG};

	std::string interface_str = "(";
	std::vector<std::shared_ptr<Object>> args{};
	for (uint8_t i = 0; i < vA; ++i) {
		auto obj = frame.getObjRegister(regs[i]);
		if (i == 0) {
			interface_str += "this=";
		}
		interface_str += obj->debug();
		if (i < vA - 1) {
			interface_str += ", ";
		}
		args.push_back(obj);
	}
	interface_str += ")";

	auto this_ptr = std::dynamic_pointer_cast<ObjectClass>(frame.getObjRegister(regs[0]));
	if (this_ptr->isNull()) {
		throw NullPointerException("invoke_interface on null object");
	}
	auto this_ptr_class = std::dynamic_pointer_cast<ObjectClass>(this_ptr);
	if (this_ptr_class == nullptr) {
		throw std::runtime_error(fmt::format("invoke-interface: this pointer is not an ObjectClass, got {}", this_ptr->debug()));
	}
	Class* instance = &this_ptr_class->getClass();
	if (!instance->isStaticInitialized()) {
		throw std::runtime_error(fmt::format("invoke-interface: class {} is not static initialized", instance->getFullname()));
	}
	auto& interface = classloader.resolveMethod(frame.getDexIdx(), methodRef);
	if (!interface.getClass().isInterface()) {
		throw std::runtime_error(fmt::format("invoke-interface: {}.{}{} is not an interface method", interface.getClass().getFullname(), interface.getName(),
		                                     interface.getSignature()));
	}

	Method* vmethod = nullptr;
	while (1) {
		try {
			logger.debug(fmt::format("invoke-interface check class {} implements interface {}.{}{}", instance->getFullname(), interface.getName(),
			                         interface.getSignature(), interface_str));
			vmethod = &instance->getMethod(interface.getName(), interface.getSignature());
			break;  // Method found, exit loop
		} catch (std::exception& e) {
			logger.debug(fmt::format("invoke-interface: class {} does not implement interface {}.{}{} not found, trying superclass", instance->getFullname(),
			                         interface.getName(), interface.getSignature(), interface_str));
			if (instance->hasSuperClass()) {
				// If the method is not found in the current class, try the superclass
				instance = &classloader.getOrLoad(instance->getSuperClassname());
				if (!instance->isStaticInitialized()) {
					frame.pc()--;
					executeClinit(*instance);
					return;
				}
				if (instance->isInterface()) {
					// If the superclass is an interface, we need to check its methods
					try {
						vmethod = &instance->getMethod(interface.getName(), interface.getSignature());
						break;  // Method found, exit loop
					} catch (std::exception& e) {
						// Continue searching in the next superclass
					}
				}
			} else {
				// If no superclass, break the loop
				break;
			}
		}
	}
	if (vmethod) {
		if (!vmethod->isVirtual()) {
			logger.error(fmt::format("invoke-interface: method {}->{}{} is not virtual", this_ptr_class->getClass().getFullname(), interface.getName(),
			                         interface.getSignature()));
		}
		if (vmethod->isNative()) {
			executeNativeMethod(*vmethod, args);
		} else {
			logger.ok(fmt::format("invoke-interface call method {}->{}{}{} on instance {}", instance->getFullname(), interface.getName(),
			                      interface.getSignature(), interface_str, this_ptr_class->getClass().getFullname()));
			auto& newframe = _rt.newFrame(*vmethod);
			// When a method is invoked, the parameters to the method are placed into the last n registers.
			for (uint32_t i = 0; i < vA; i++) {
				newframe.setObjRegister(vmethod->getNbRegisters() - vA + i, frame.getObjRegister(regs[i]));
			}
		}
	} else {
		// If no method found, throw an error
		throw std::runtime_error(fmt::format("invoke-interface: call method {}->{}{}{} not found", this_ptr_class->getClass().getFullname(),
		                                     interface.getName(), interface.getSignature(), interface_str));
	}
	frame.pc() += 5;
}
// invoke-virtual/range {vCCCC .. vNNNN}, meth@BBBB
void Interpreter::invoke_virtual_range(const uint8_t* operand_) {
	throw std::runtime_error("invoke_virtual_range not implemented");
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
		frame.pc()--;
		executeClinit(cls);
		return;
	}

	std::vector<std::shared_ptr<Object>> args;
	for (uint8_t i = 0; i < regCount; ++i) {
		args.push_back(frame.getObjRegister(startReg + i));
	}

	if (method.isNative()) {
		executeNativeMethod(method, args);
	} else {
		if (method.getBytecode() == nullptr) {
			method.execute(frame, args);
		} else {
			logger.ok(fmt::format("invoke-static-range call method {}.{}{} with {} arguments", method.getClass().getFullname(), method.getName(),
			                      method.getSignature(), regCount));
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
	throw std::runtime_error("invoke_interface_range not implemented");
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
		throw std::runtime_error("Division by zero in div-int");
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
		throw std::runtime_error("Division by zero in rem-int");
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
		throw std::runtime_error("Division by zero in div-long");
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
		throw std::runtime_error("Division by zero in rem-long");
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
		throw std::runtime_error("Division by zero in div-float");
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
		throw std::runtime_error("Division by zero in rem-float");
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
		throw std::runtime_error("Division by zero in div-double");
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
		throw std::runtime_error("Division by zero in rem-double");
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
		throw std::runtime_error("Division by zero in div-int/2addr");
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
		throw std::runtime_error("Division by zero in rem-int/2addr");
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
		throw std::runtime_error("Division by zero in div-long/2addr");
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
		throw std::runtime_error("Division by zero in rem-long/2addr");
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
		throw std::runtime_error("Division by zero in div-float/2addr");
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
		throw std::runtime_error("Division by zero in rem-float/2addr");
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
		throw std::runtime_error("Division by zero in div-double/2addr");
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
		throw std::runtime_error("Division by zero in rem-double/2addr");
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
		throw std::runtime_error("Division by zero in div-int/lit16");
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
		throw std::runtime_error("Division by zero in rem-int/lit16");
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
		throw std::runtime_error("Division by zero in div-int/lit8");
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
		throw std::runtime_error("Division by zero in rem-int/lit8");
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
