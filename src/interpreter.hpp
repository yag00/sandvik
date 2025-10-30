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

#ifndef __INTERPRETER_HPP__
#define __INTERPRETER_HPP__

#include <stdint.h>

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace sandvik {
	class Disassembler;
	class Method;
	class Object;
	class Class;
	class JThread;
	/** @brief Interpreter class
	 */
	class Interpreter {
		public:
			/** Create opcode interpreter for the given thread
			 * @param rt_ reference to the current thread
			 */
			explicit Interpreter(JThread& rt_);
			~Interpreter();

			/** @brief executes current thread opcode. */
			void execute();

		private:
			/** Opcode method declarations */
			// nop
			void nop(const uint8_t* operand_);
			// move vA, vB
			void move(const uint8_t* operand_);
			// move/from16 vAA, vBBBB
			void move_from16(const uint8_t* operand_);
			// move/16 vAAAA, vBBBB
			void move_16(const uint8_t* operand_);
			// move-wide vA, vB
			void move_wide(const uint8_t* operand_);
			// move-wide/from16 vAA, vBBBB
			void move_wide_from16(const uint8_t* operand_);
			// move-wide/16 vAAAA, vBBBB
			void move_wide16(const uint8_t* operand_);
			// move-object vA, vB
			void move_object(const uint8_t* operand_);
			// move-object/from16 vAA, vBBBB
			void move_object_from16(const uint8_t* operand_);
			// move-object/16 vAAAA, vBBBB
			void move_object16(const uint8_t* operand_);
			//  move-result vAA
			void move_result(const uint8_t* operand_);
			// move-result-wide vAA
			void move_result_wide(const uint8_t* operand_);
			// move-result-object vAA
			void move_result_object(const uint8_t* operand_);
			// move-exception vAA
			void move_exception(const uint8_t* operand_);
			// return-void
			void return_void(const uint8_t* operand_);
			// return vAA
			void return_(const uint8_t* operand_);
			// return-wide vAA
			void return_wide(const uint8_t* operand_);
			// return-object vAA
			void return_object(const uint8_t* operand_);
			// const/4 vA, #+B
			void const_4(const uint8_t* operand_);
			// const/16 vAA, #+BBBB
			void const_16(const uint8_t* operand_);
			// const vAA, #+BBBBBBBB
			void const_(const uint8_t* operand_);
			// const/high16 vAA, #+BBBB0000
			void const_high16(const uint8_t* operand_);
			// const-wide/16 vAA, #+BBBB
			void const_wide_16(const uint8_t* operand_);
			// const-wide/32 vAA, #+BBBBBBBB
			void const_wide_32(const uint8_t* operand_);
			// const-wide vAA, #+BBBBBBBBBBBBBBBB
			void const_wide(const uint8_t* operand_);
			// const-wide/high16 vAA, #+BBBB000000000000
			void const_wide_high16(const uint8_t* operand_);
			// const-string vAA, string@BBBB
			void const_string(const uint8_t* operand_);
			// const-string/jumbo vAA, string@BBBBBBBB
			void const_string_jumbo(const uint8_t* operand_);
			// const-class vAA, type@BBBB
			void const_class(const uint8_t* operand_);
			// monitor-enter vAA
			void monitor_enter(const uint8_t* operand_);
			// monitor-exit vAA
			void monitor_exit(const uint8_t* operand_);
			// check-cast vAA, type@BBBB
			void check_cast(const uint8_t* operand_);
			// instance-of vA, vB, type@CCCC
			void instance_of(const uint8_t* operand_);
			// array-length vA, vB
			void array_length(const uint8_t* operand_);
			// new-instance vAA, type@BBBB
			void new_instance(const uint8_t* operand_);
			// new-array vA, vB, type@CCCC
			void new_array(const uint8_t* operand_);
			// filled-new-array {vD, vE, vF, vG, vA}, type@CCCC
			void filled_new_array(const uint8_t* operand_);
			// filled-new-array/range {vCCCC .. vNNNN}, type@BBBB
			void filled_new_array_range(const uint8_t* operand_);
			// fill-array-data vAA, +BBBBBBBB
			void fill_array_data(const uint8_t* operand_);
			// throw vAA
			void throw_(const uint8_t* operand_);
			// goto +AA
			void goto_(const uint8_t* operand_);
			// goto/16 +AAAA
			void goto_16(const uint8_t* operand_);
			// goto/32 +AAAAAAAA
			void goto_32(const uint8_t* operand_);
			// packed-switch vAA, +BBBBBBBB
			void packed_switch(const uint8_t* operand_);
			// sparse-switch vAA, +BBBBBBBB
			void sparse_switch(const uint8_t* operand_);
			// cmpl-float vAA, vBB, vCC
			void cmpl_float(const uint8_t* operand_);
			// cmpg-float vAA, vBB, vCC
			void cmpg_float(const uint8_t* operand_);
			// cmpl-double vAA, vBB, vCC
			void cmpl_double(const uint8_t* operand_);
			// cmpg-double vAA, vBB, vCC
			void cmpg_double(const uint8_t* operand_);
			// cmp-long vAA, vBB, vCC
			void cmp_long(const uint8_t* operand_);
			// if-eq vA, vB, +CCCC
			void if_eq(const uint8_t* operand_);
			// if-ne vA, vB, +CCCC
			void if_ne(const uint8_t* operand_);
			// if-lt vA, vB, +CCCC
			void if_lt(const uint8_t* operand_);
			// if-ge vA, vB, +CCCC
			void if_ge(const uint8_t* operand_);
			// if-gt vA, vB, +CCCC
			void if_gt(const uint8_t* operand_);
			// if-le vA, vB, +CCCC
			void if_le(const uint8_t* operand_);
			// if-eqz vAA, +BBBB
			void if_eqz(const uint8_t* operand_);
			// if-nez vAA, +BBBB
			void if_nez(const uint8_t* operand_);
			// if-ltz vAA, +BBBB
			void if_ltz(const uint8_t* operand_);
			// if-gez vAA, +BBBB
			void if_gez(const uint8_t* operand_);
			// if-gtz vAA, +BBBB
			void if_gtz(const uint8_t* operand_);
			// if-lez vAA, +BBBB
			void if_lez(const uint8_t* operand_);
			// aget vAA, vBB, vCC
			void aget(const uint8_t* operand_);
			// aget-wide vAA, vBB, vCC
			void aget_wide(const uint8_t* operand_);
			// aget-object vAA, vBB, vCC
			void aget_object(const uint8_t* operand_);
			// aget-boolean vAA, vBB, vCC
			void aget_boolean(const uint8_t* operand_);
			// aget-byte vAA, vBB, vCC
			void aget_byte(const uint8_t* operand_);
			// aget-char vAA, vBB, vCC
			void aget_char(const uint8_t* operand_);
			// aget-short vAA, vBB, vCC
			void aget_short(const uint8_t* operand_);
			// aput vAA, vBB, vCC
			void aput(const uint8_t* operand_);
			// aput-wide vAA, vBB, vCC
			void aput_wide(const uint8_t* operand_);
			// aput-object vAA, vBB, vCC
			void aput_object(const uint8_t* operand_);
			// aput-boolean vAA, vBB, vCC
			void aput_boolean(const uint8_t* operand_);
			// aput-byte vAA, vBB, vCC
			void aput_byte(const uint8_t* operand_);
			// aput-char vAA, vBB, vCC
			void aput_char(const uint8_t* operand_);
			// aput-short vAA, vBB, vCC
			void aput_short(const uint8_t* operand_);
			// iget vA, vB, field@CCCC
			void iget(const uint8_t* operand_);
			// iget-wide vA, vB, field@CCCC
			void iget_wide(const uint8_t* operand_);
			// iget-object vA, vB, field@CCCC
			void iget_object(const uint8_t* operand_);
			// iget-boolean vA, vB, field@CCCC
			void iget_boolean(const uint8_t* operand_);
			// iget-byte vA, vB, field@CCCC
			void iget_byte(const uint8_t* operand_);
			// iget-char vA, vB, field@CCCC
			void iget_char(const uint8_t* operand_);
			// iget-short vA, vB, field@CCCC
			void iget_short(const uint8_t* operand_);
			// iput vA, vB, field@CCCC
			void iput(const uint8_t* operand_);
			// iput-wide vA, vB, field@CCCC
			void iput_wide(const uint8_t* operand_);
			// iput-object vA, vB, field@CCCC
			void iput_object(const uint8_t* operand_);
			// iput-boolean vA, vB, field@CCCC
			void iput_boolean(const uint8_t* operand_);
			// iput-byte vA, vB, field@CCCC
			void iput_byte(const uint8_t* operand_);
			// iput-char vA, vB, field@CCCC
			void iput_char(const uint8_t* operand_);
			// iput-short vA, vB, field@CCCC
			void iput_short(const uint8_t* operand_);
			// sget vA, field@BBBB
			void sget(const uint8_t* operand_);
			// sget-wide vA, field@BBBB
			void sget_wide(const uint8_t* operand_);
			// sget-object vA, field@BBBB
			void sget_object(const uint8_t* operand_);
			// sget-boolean vA, field@BBBB
			void sget_boolean(const uint8_t* operand_);
			// sget-byte vA, field@BBBB
			void sget_byte(const uint8_t* operand_);
			// sget-char vA, field@BBBB
			void sget_char(const uint8_t* operand_);
			// sget-short vA, field@BBBB
			void sget_short(const uint8_t* operand_);
			// sput vA, field@BBBB
			void sput(const uint8_t* operand_);
			// sput-wide vA, field@BBBB
			void sput_wide(const uint8_t* operand_);
			// sput-object vA, field@BBBB
			void sput_object(const uint8_t* operand_);
			// sput-boolean vA, field@BBBB
			void sput_boolean(const uint8_t* operand_);
			// sput-byte vA, field@BBBB
			void sput_byte(const uint8_t* operand_);
			// sput-char vA, field@BBBB
			void sput_char(const uint8_t* operand_);
			// sput-short vA, field@BBBB
			void sput_short(const uint8_t* operand_);
			// invoke-virtual {vD, vE, vF, vG, vA}, meth@CCCC
			void invoke_virtual(const uint8_t* operand_);
			// invoke-super {vD, vE, vF, vG, vA}, meth@CCCC
			void invoke_super(const uint8_t* operand_);
			// invoke-direct {vD, vE, vF, vG, vA}, meth@CCCC
			void invoke_direct(const uint8_t* operand_);
			// invoke-static {vD, vE, vF, vG, vA}, meth@CCCC
			void invoke_static(const uint8_t* operand_);
			// invoke-interface {vD, vE, vF, vG, vA}, meth@CCCC
			void invoke_interface(const uint8_t* operand_);
			// invoke-virtual/range {vCCCC .. vNNNN}, meth@BBBB
			void invoke_virtual_range(const uint8_t* operand_);
			// invoke-super/range {vCCCC .. vNNNN}, meth@BBBB
			void invoke_super_range(const uint8_t* operand_);
			// invoke-direct/range {vCCCC .. vNNNN}, meth@BBBB
			void invoke_direct_range(const uint8_t* operand_);
			// invoke-static/range {vCCCC .. vNNNN}, meth@BBBB
			void invoke_static_range(const uint8_t* operand_);
			// invoke-interface/range {vCCCC .. vNNNN}, meth@BBBB
			void invoke_interface_range(const uint8_t* operand_);
			// neg-int vA, vB
			void neg_int(const uint8_t* operand_);
			// not-int vA, vB
			void not_int(const uint8_t* operand_);
			// neg-long vA, vB
			void neg_long(const uint8_t* operand_);
			// not-long vA, vB
			void not_long(const uint8_t* operand_);
			// neg-float vA, vB
			void neg_float(const uint8_t* operand_);
			// neg-double vA, vB
			void neg_double(const uint8_t* operand_);
			// int-to-long
			void int_to_long(const uint8_t* operand_);
			// int-to-float vA, vB
			void int_to_float(const uint8_t* operand_);
			// int-to-double vA, vB
			void int_to_double(const uint8_t* operand_);
			// long-to-int
			void long_to_int(const uint8_t* operand_);
			// long-to-float
			void long_to_float(const uint8_t* operand_);
			// long-to-double
			void long_to_double(const uint8_t* operand_);
			// float-to-int vA, vB
			void float_to_int(const uint8_t* operand_);
			// float-to-long vA, vB
			void float_to_long(const uint8_t* operand_);
			// float-to-double vA, vB
			void float_to_double(const uint8_t* operand_);
			// double-to-int vA, vB
			void double_to_int(const uint8_t* operand_);
			// double-to-long vA, vB
			void double_to_long(const uint8_t* operand_);
			// double-to-float vA, vB
			void double_to_float(const uint8_t* operand_);
			// int-to-byte
			void int_to_byte(const uint8_t* operand_);
			// int-to-char
			void int_to_char(const uint8_t* operand_);
			// int-to-short
			void int_to_short(const uint8_t* operand_);
			// add-int vAA, vBB, vCC
			void add_int(const uint8_t* operand_);
			// sub-int vAA, vBB, vCC
			void sub_int(const uint8_t* operand_);
			// mul-int vAA, vBB, vCC
			void mul_int(const uint8_t* operand_);
			// div-int vAA, vBB, vCC
			void div_int(const uint8_t* operand_);
			// rem-int vAA, vBB, vCC
			void rem_int(const uint8_t* operand_);
			// and-int vAA, vBB, vCC
			void and_int(const uint8_t* operand_);
			// or-int vAA, vBB, vCC
			void or_int(const uint8_t* operand_);
			// xor-int vAA, vBB, vCC
			void xor_int(const uint8_t* operand_);
			// shl-int vAA, vBB, vCC
			void shl_int(const uint8_t* operand_);
			// shr-int vAA, vBB, vCC
			void shr_int(const uint8_t* operand_);
			// ushr-int vAA, vBB, vCC
			void ushr_int(const uint8_t* operand_);
			// add-long vAA, vBB, vCC
			void add_long(const uint8_t* operand_);
			// sub-long vAA, vBB, vCC
			void sub_long(const uint8_t* operand_);
			// mul-long
			void mul_long(const uint8_t* operand_);
			// div-long
			void div_long(const uint8_t* operand_);
			// rem-long
			void rem_long(const uint8_t* operand_);
			// and-long
			void and_long(const uint8_t* operand_);
			// or-long
			void or_long(const uint8_t* operand_);
			// xor-long
			void xor_long(const uint8_t* operand_);
			// shl-long
			void shl_long(const uint8_t* operand_);
			// shr-long
			void shr_long(const uint8_t* operand_);
			// ushr-long
			void ushr_long(const uint8_t* operand_);
			// add-float vAA, vBB, vCC
			void add_float(const uint8_t* operand_);
			// sub-float vAA, vBB, vCC
			void sub_float(const uint8_t* operand_);
			// mul-float vAA, vBB, vCC
			void mul_float(const uint8_t* operand_);
			// div-float vAA, vBB, vCC
			void div_float(const uint8_t* operand_);
			// rem-float vAA, vBB, vCC
			void rem_float(const uint8_t* operand_);
			// add-double vAA, vBB, vCC
			void add_double(const uint8_t* operand_);
			// sub-double vAA, vBB, vCC
			void sub_double(const uint8_t* operand_);
			// mul-double vAA, vBB, vCC
			void mul_double(const uint8_t* operand_);
			// div-double vAA, vBB, vCC
			void div_double(const uint8_t* operand_);
			// rem-double vAA, vBB, vCC
			void rem_double(const uint8_t* operand_);
			// add-int/2addr vA, vB
			void add_int_2addr(const uint8_t* operand_);
			// sub-int/2addr vA, vB
			void sub_int_2addr(const uint8_t* operand_);
			// mul-int/2addr vA, vB
			void mul_int_2addr(const uint8_t* operand_);
			// div-int/2addr vA, vB
			void div_int_2addr(const uint8_t* operand_);
			// rem-int/2addr vA, vB
			void rem_int_2addr(const uint8_t* operand_);
			// and-int/2addr vA, vB
			void and_int_2addr(const uint8_t* operand_);
			// or-int/2addr vA, vB
			void or_int_2addr(const uint8_t* operand_);
			// xor-int/2addr vA, vB
			void xor_int_2addr(const uint8_t* operand_);
			// shl-int/2addr vA, vB
			void shl_int_2addr(const uint8_t* operand_);
			// shr-int/2addr vA, vB
			void shr_int_2addr(const uint8_t* operand_);
			// ushr-int/2addr vA, vB
			void ushr_int_2addr(const uint8_t* operand_);
			// add-long/2addr vA, vB
			void add_long_2addr(const uint8_t* operand_);
			// sub-long/2addr vA, vB
			void sub_long_2addr(const uint8_t* operand_);
			// mul-long/2addr vA, vB
			void mul_long_2addr(const uint8_t* operand_);
			// div-long/2addr vA, vB
			void div_long_2addr(const uint8_t* operand_);
			// rem-long/2addr vA, vB
			void rem_long_2addr(const uint8_t* operand_);
			// and-long/2addr vA, vB
			void and_long_2addr(const uint8_t* operand_);
			// or-long/2addr vA, vB
			void or_long_2addr(const uint8_t* operand_);
			// xor-long/2addr vA, vB
			void xor_long_2addr(const uint8_t* operand_);
			// shl-long/2addr vA, vB
			void shl_long_2addr(const uint8_t* operand_);
			// shr-long/2addr vA, vB
			void shr_long_2addr(const uint8_t* operand_);
			// ushr-long/2addr vA, vB
			void ushr_long_2addr(const uint8_t* operand_);
			// add-float/2addr vA, vB
			void add_float_2addr(const uint8_t* operand_);
			// sub-float/2addr vA, vB
			void sub_float_2addr(const uint8_t* operand_);
			// mul-float/2addr vA, vB
			void mul_float_2addr(const uint8_t* operand_);
			// div-float/2addr vA, vB
			void div_float_2addr(const uint8_t* operand_);
			// rem-float/2addr vA, vB
			void rem_float_2addr(const uint8_t* operand_);
			// add-double/2addr vA, vB
			void add_double_2addr(const uint8_t* operand_);
			// sub-double/2addr vA, vB
			void sub_double_2addr(const uint8_t* operand_);
			// mul-double/2addr vA, vB
			void mul_double_2addr(const uint8_t* operand_);
			// div-double/2addr vA, vB
			void div_double_2addr(const uint8_t* operand_);
			// rem-double/2addr vA, vB
			void rem_double_2addr(const uint8_t* operand_);
			// add-int/lit16 vA, vB, #+CCCC
			void add_int_lit16(const uint8_t* operand_);
			// rsub-int/lit16 vA, vB, #+CCCC
			void rsub_int_lit16(const uint8_t* operand_);
			// mul-int/lit16 vA, vB, #+CCCC
			void mul_int_lit16(const uint8_t* operand_);
			// div-int/lit16 vA, vB, #+CCCC
			void div_int_lit16(const uint8_t* operand_);
			// rem-int/lit16 vA, vB, #+CCCC
			void rem_int_lit16(const uint8_t* operand_);
			// and-int/lit16 vA, vB, #+CCCC
			void and_int_lit16(const uint8_t* operand_);
			// or-int/lit16 vA, vB, #+CCCC
			void or_int_lit16(const uint8_t* operand_);
			// xor-int/lit16 vA, vB, #+CCCC
			void xor_int_lit16(const uint8_t* operand_);
			// add-int/lit8 vAA, vBB, #+CC
			void add_int_lit8(const uint8_t* operand_);
			// rsub-int/lit8 vAA, vBB, #+CC
			void rsub_int_lit8(const uint8_t* operand_);
			// mul-int/lit8 vAA, vBB, #+CC
			void mul_int_lit8(const uint8_t* operand_);
			// div-int/lit8 vAA, vBB, #+CC
			void div_int_lit8(const uint8_t* operand_);
			// rem-int/lit8 vAA, vBB, #+CC
			void rem_int_lit8(const uint8_t* operand_);
			// and-int/lit8 vAA, vBB, #+CC
			void and_int_lit8(const uint8_t* operand_);
			// or-int/lit8 vAA, vBB, #+CC
			void or_int_lit8(const uint8_t* operand_);
			// xor-int/lit8 vAA, vBB, #+CC
			void xor_int_lit8(const uint8_t* operand_);
			// shl-int/lit8 vAA, vBB, #+CC
			void shl_int_lit8(const uint8_t* operand_);
			// shr-int/lit8 vAA, vBB, #+CC
			void shr_int_lit8(const uint8_t* operand_);
			// ushr-int/lit8 vAA, vBB, #+CC
			void ushr_int_lit8(const uint8_t* operand_);

			std::vector<std::function<void(const uint8_t* operand_)>> _dispatch;

			void handleException(std::shared_ptr<Object> exception_);
			void executeClinit(Class& class_) const;
			void executeNativeMethod(const Method& method_, const std::vector<std::shared_ptr<Object>>& args_);

		private:
			JThread& _rt;
			std::unique_ptr<Disassembler> _disassembler;

			std::map<uint8_t, uint64_t> _instcoverage;
	};
}  // namespace sandvik

#endif