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
	class Frame {
		public:
			explicit Frame(Method& method_);
			~Frame() = default;

			uint32_t getDexIdx() const;

			void setIntRegister(uint32_t reg, int32_t value);
			int32_t getIntRegister(uint32_t reg) const;
			void setLongRegister(uint32_t reg, int64_t value);
			int64_t getLongRegister(uint32_t reg) const;
			void setFloatRegister(uint32_t reg, float value);
			float getFloatRegister(uint32_t reg) const;
			void setDoubleRegister(uint32_t reg, double value);
			double getDoubleRegister(uint32_t reg) const;

			void setObjRegister(uint32_t reg, std::shared_ptr<Object> value);
			std::shared_ptr<Object> getObjRegister(uint32_t reg);

			std::shared_ptr<Object> getReturnObject() const;
			std::shared_ptr<Object> getException() const;
			int32_t getReturnValue() const;
			int64_t getReturnDoubleValue() const;
			void setReturnObject(std::shared_ptr<Object> ret_);
			void setReturnValue(int32_t ret_);
			void setReturnDoubleValue(int64_t ret_);
			void setException(std::shared_ptr<Object> exception_);

			uint16_t pc() const;
			uint16_t& pc();
			void setPc(uint16_t pc_);

			Method& getMethod() const;

			void debug() const;

		protected:
			void increaseRegSize(uint32_t size_);

		private:
			Method& _method;
			std::vector<std::string> _args;

			std::vector<std::shared_ptr<Object>> _registers;

			uint16_t _pc;

			std::shared_ptr<Object> _null;
			std::shared_ptr<Object> _objectReturn;
			std::shared_ptr<Object> _exception;
	};
}  // namespace sandvik

#endif