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

#ifndef __TRACE_HPP__
#define __TRACE_HPP__

#include <stdarg.h>
#include <stdint.h>

#include <fmt/format.h>

#include <memory>
#include <string>
#include <system/singleton.hpp>
#include <vector>

#include "object.hpp"

#define trace ::sandvik::getTrace()

namespace sandvik {
	class Frame;
	class Disassembler;
	/** @brief Trace class
	 */
	class Trace : public Singleton<Trace> {
		public:
			/** Enable/disable traces
			 * @param enable_ enable/disable
			 */
			void enableInstructionTrace(bool enable_);
			/** Enable/disable traces
			 * @param enable_ enable/disable
			 */
			void enableCallTrace(bool enable_);

			/** log instruction trace entry.
			 * @param pc_ program counter
			 * @param function_ function name and signature
			 * @param bytecode_ opcode
			 */
			void logInstruction(const uint64_t pc_, const std::string& function_, const uint8_t* bytecode_);
			/** log call trace entry.
			 * @param type_ invoke type
			 * @param class_ class name
			 * @param method_ method name
			 * @param signature_ signature
			 * @param args_ arguments string
			 * @param static_ method is static
			 */
			void logCall(const std::string& type_, const std::string& class_, const std::string& method_, const std::string& signature_,
			             const std::vector<ObjectRef>& args_, bool static_);

		private:
			friend class Singleton<Trace>;
			Trace();

			bool _trace_instructions;
			bool _trace_calls;

			std::unique_ptr<Disassembler> _disassembler;
	};
	/** @brief Get the global trace instance
	 * @return Reference to the global trace instance
	 */
	inline Trace& getTrace() {
		return Trace::getInstance();
	}
}  // namespace sandvik

#endif
