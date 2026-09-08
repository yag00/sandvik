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

#ifndef __SANDVIK_DEX_CODEINFO_HPP__
#define __SANDVIK_DEX_CODEINFO_HPP__

#include <cstdint>
#include <utility>
#include <vector>

namespace sandvik {
	namespace dex {
		/** @brief One try/catch entry decoded from a code_item's tries/handlers tables.
		 */
		struct trycatch_item {
				uint32_t start_addr;                                  ///< First code-unit address covered by this try block.
				uint32_t insn_count;                                  ///< Number of code units covered, starting at start_addr.
				std::vector<std::pair<uint32_t, uint32_t>> handlers;  ///< (type_idx, handler_addr) pairs
				uint32_t catch_all_addr;                              ///< Catch-all handler address, or 0 if there is none.
		};

		/** @brief Per-method metadata decoded from its code_item: register count and exception
		 * handler table.
		 */
		class CodeInfo {
			public:
				CodeInfo() = default;

				/** @brief Total number of registers this method's code uses. */
				uint16_t nb_registers() const {
					return _nbRegisters;
				}
				/** @brief Sets the register count (called while decoding the code_item). */
				void setNbRegisters(uint16_t v_) {
					_nbRegisters = v_;
				}

				/** @brief This method's decoded try/catch table, in code_item order. */
				const std::vector<trycatch_item>& exceptions() const {
					return _tryCatchItems;
				}
				/** @brief Mutable access to the try/catch table (populated while decoding the code_item). */
				std::vector<trycatch_item>& exceptions() {
					return _tryCatchItems;
				}

			private:
				uint16_t _nbRegisters = 0;
				std::vector<trycatch_item> _tryCatchItems;
		};
	}  // namespace dex
}  // namespace sandvik

#endif  // __SANDVIK_DEX_CODEINFO_HPP__
