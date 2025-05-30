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
	class String {
		public:
			static void equals(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				auto str0 = std::dynamic_pointer_cast<StringObject>(args_[0]);
				auto str1 = std::dynamic_pointer_cast<StringObject>(args_[1]);
				if (str0 != nullptr && str1 != nullptr) {
					if (str0->str() == str1->str()) {
						frame_.setReturnValue(1);
					} else {
						frame_.setReturnValue(0);
					}
				} else {
					throw std::runtime_error("First or second argument is not an instance of java.lang.String");
				}
			}
			static void length(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 1) {
					throw std::runtime_error("Not enough arguments");
				}
				auto clazz = std::dynamic_pointer_cast<StringObject>(args_[0]);
				if (!clazz) {
					throw std::runtime_error("First argument is not an instance of java.lang.String");
				}
				frame_.setReturnValue(clazz->str().length());
			}
	};
}  // namespace

namespace java {
	namespace lang {
		void String(::sandvik::ClassLoader& classLoader) {
			ClassBuilder builder(classLoader, "java.lang", "java.lang.String");
			builder.setSuperClass("java.lang.Object");
			builder.addVirtualMethod("equals", "(Ljava/lang/Object;)Z", 0, String::equals);
			builder.addVirtualMethod("length", "()I", 0, String::length);
			builder.finalize();
		}
	}  // namespace lang
}  // namespace java