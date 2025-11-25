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

#ifndef __DISASSEMBLER_HPP__
#define __DISASSEMBLER_HPP__

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace sandvik {
	/** @brief Disassembler for DEX bytecode. */
	class Disassembler {
		public:
			Disassembler();
			~Disassembler() = default;

			/** @brief return the name of an opcode
			 * @param opcode_ Opcode byte.
			 * @return String representation of the opcode.
			 */
			std::string disassemble(const uint8_t opcode_) const;
			/** @brief Disassembles one opcode
			 * @param bytecode_ Pointer to the bytecode.
			 * @param size_ Reference to size variable, updated with the size of disassembled code.
			 * @return String representation of the disassembled bytecode.
			 */
			std::string disassemble(const uint8_t* bytecode_, uint32_t& size_) const;
			/** @brief Disassembles one opcode
			 * @param bytecode_ Pointer to the bytecode.
			 * @return String representation of the disassembled bytecode.
			 */
			std::string disassemble(const uint8_t* bytecode_) const;

		private:
			std::string format_i10x(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i12x(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i11n(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i11x(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i10t(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i20t(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i22x(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i21t(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i21s(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i21h(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i21c(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i23x(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i22b(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i22t(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i22s(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i22c(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i25c(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i30t(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i32x(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i31i(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i31t(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i31c(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i35c(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i3rc(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;
			std::string format_i51l(const std::string& name_, const uint8_t* operand_, uint32_t& size_) const;

			std::vector<std::function<std::string(const std::string&, const uint8_t*, uint32_t&)>> _dispatch;
	};
}  // namespace sandvik

#endif  // __DISASSEMBLER_HPP__