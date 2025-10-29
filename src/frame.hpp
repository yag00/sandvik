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

#ifndef __FRAME_HPP__
#define __FRAME_HPP__

#include <stdint.h>

#include <memory>
#include <stack>
#include <vector>

#include "object.hpp"

namespace sandvik {
	class Method;
	class Object;
	/** @class Frame
	 *  @brief Represents a stack frame for method execution.
	 *
	 *  The Frame class encapsulates the state of a method invocation,
	 *  including local registers, program counter, and return values.
	 */
	class Frame {
		public:
			/** @brief Constructs a new Frame for the given method.
			 *  @param method_ Reference to the Method associated with this frame.
			 */
			explicit Frame(Method& method_);
			~Frame() = default;

			/** @brief Gets the Dex index of the method
			 *  @return Dex index.
			 */
			uint32_t getDexIdx() const;

			/** @brief Sets the integer (32-bit) register value.
			 *  @param reg Register index.
			 *  @param value Value to set.
			 */
			void setIntRegister(uint32_t reg, int32_t value);
			/** @brief Gets the integer (32-bit) register value.
			 *  @param reg Register index.
			 *  @return Value of the register.
			 */
			int32_t getIntRegister(uint32_t reg) const;
			/** @brief Sets the long (64-bit) register value.
			 *  @param reg Register index.
			 *  @param value Value to set.
			 */
			void setLongRegister(uint32_t reg, int64_t value);
			/** @brief Gets the long (64-bit) register value.
			 *  @param reg Register index.
			 *  @return Value of the register.
			 */
			int64_t getLongRegister(uint32_t reg) const;
			/** @brief Sets the float (32-bit) register value.
			 *  @param reg Register index.
			 *  @param value Value to set.
			 */
			void setFloatRegister(uint32_t reg, float value);
			/** @brief Gets the float (32-bit) register value.
			 *  @param reg Register index.
			 *  @return Value of the register.
			 */
			float getFloatRegister(uint32_t reg) const;
			/** @brief Sets the double (64-bit) register value.
			 *  @param reg Register index.
			 *  @param value Value to set.
			 */
			void setDoubleRegister(uint32_t reg, double value);
			/** @brief Gets the double (64-bit) register value.
			 *  @param reg Register index.
			 *  @return Value of the register.
			 */
			double getDoubleRegister(uint32_t reg) const;

			/** @brief Sets the Object register value.
			 *  @param reg Register index.
			 *  @param value Value to set.
			 */
			void setObjRegister(uint32_t reg, std::shared_ptr<Object> value);
			/** @brief Gets the Object register value.
			 *  @param reg Register index.
			 *  @return Value of the register.
			 */
			std::shared_ptr<Object> getObjRegister(uint32_t reg);

			/** @brief Gets the return Object value.
			 *  @return Return Object.
			 */
			std::shared_ptr<Object> getReturnObject() const;
			/** @brief Gets the exception Object.
			 *  @return Exception Object.
			 */
			std::shared_ptr<Object> getException() const;
			/** @brief Gets the integer (32-bit) return value.
			 *  @return Return value.
			 */
			int32_t getReturnValue() const;
			/** @brief Gets the long (64-bit) return value.
			 *  @return Return long value.
			 */
			int64_t getReturnDoubleValue() const;
			/** @brief Sets the return Object value.
			 *  @param ret_ Shared pointer to the return Object.
			 */
			void setReturnObject(std::shared_ptr<Object> ret_);
			/** @brief Sets the integer (32-bit) return value.
			 *  @param ret_ Return value.
			 */
			void setReturnValue(int32_t ret_);
			/** @brief Sets the long (64-bit) return value.
			 *  @param ret_ Return long value.
			 */
			void setReturnDoubleValue(int64_t ret_);
			/** @brief Sets the exception Object.
			 *  @param exception_ Shared pointer to the exception Object.
			 */
			void setException(std::shared_ptr<Object> exception_);

			/** @brief Gets the program counter.
			 *  @return Program counter.
			 */
			uint16_t pc() const;
			/** @brief Gets a reference to the program counter.
			 *  @return Reference to the program counter.
			 */
			uint16_t& pc();
			/** @brief Sets the program counter.
			 *  @param pc_ Program counter value to set.
			 */
			void setPc(uint16_t pc_);

			/** @brief Gets the Method associated with this frame.
			 *  @return Reference to the Method.
			 */
			Method& getMethod() const;

			/** @brief Outputs a debug representation of the frame. */
			void debug() const;

		protected:
			/** @brief Increases the size of the register array.
			 *  @param size_ New size of the register array.
			 */
			void increaseRegSize(uint32_t size_);
			/** @brief Gets the raw integer (64-bit) register value.
			 *  @param reg Register index.
			 *  @return Value of the register.
			 */
			uint64_t getRawLongRegister(uint32_t reg) const;
			/** @brief Sets the raw integer (64-bit) register value.
			 *  @param reg Register index.
			 *  @param value Value to set.
			 */
			void setRawLongRegister(uint32_t reg, uint64_t value);

		private:
			Method& _method;
			std::vector<std::string> _args;

			std::vector<std::shared_ptr<Object>> _registers;

			uint16_t _pc = 0;

			std::shared_ptr<Object> _null;
			std::shared_ptr<Object> _objectReturn;
			std::shared_ptr<Object> _exception;
	};
}  // namespace sandvik

#endif