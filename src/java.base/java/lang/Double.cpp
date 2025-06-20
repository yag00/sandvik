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

#include <fmt/format.h>

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "class.hpp"
#include "classbuilder.hpp"
#include "classloader.hpp"
#include "field.hpp"
#include "frame.hpp"
#include "java.base/classes.hpp"
#include "method.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

namespace {
	class Double {
		public:
			static void isNaN(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 2) {
					throw std::runtime_error("Not enough arguments for isNaN");
				}
				auto lsb = std::dynamic_pointer_cast<NumberObject>(args_[0]);
				auto msb = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				if (!msb || !lsb) {
					throw std::runtime_error("Arguments must be numbers");
				}
				uint64_t value = (static_cast<uint64_t>((uint32_t)msb->getValue()) << 32) | static_cast<uint64_t>((uint32_t)lsb->getValue());
				double dvalue = *reinterpret_cast<double*>(&value);
				if (std::isnan(dvalue)) {
					frame_.setReturnValue(1);
				} else {
					frame_.setReturnValue(0);
				}
			}
	};
}  // namespace

namespace java {
	namespace lang {
		void Double(::sandvik::ClassLoader& classLoader) {
			ClassBuilder builder(classLoader, "java.lang", "java.lang.Double");
			builder.addMethod("isNaN", "(D)Z", ACCESS_FLAGS::ACC_STATIC, Double::isNaN);
			builder.finalize();
		}
	}  // namespace lang
}  // namespace java