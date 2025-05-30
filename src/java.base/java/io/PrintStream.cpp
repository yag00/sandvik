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
	class PrintStream {
		public:
			static void println_I(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 2) {
					throw std::runtime_error("Not enough arguments for println");
				}
				auto clazz = std::dynamic_pointer_cast<ObjectClass>(args_[0]);
				if (!clazz || !clazz->isInstanceOf("java.io.PrintStream")) {
					throw std::runtime_error("First argument is not an instance of java.io.PrintStream");
				}
				// @todo handle stream kind (System.out, System.err, etc.) not always std::cout
				auto i = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				if (i != nullptr) {
					fmt::print("{}\n", i->getValue());
					fflush(stdout);
				}
			}
			static void println_J(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 3) {
					throw std::runtime_error("Not enough arguments for println");
				}
				auto clazz = std::dynamic_pointer_cast<ObjectClass>(args_[0]);
				if (!clazz || !clazz->isInstanceOf("java.io.PrintStream")) {
					throw std::runtime_error("First argument is not an instance of java.io.PrintStream");
				}
				// @todo handle stream kind (System.out, System.err, etc.) not always std::cout
				auto msb = std::dynamic_pointer_cast<NumberObject>(args_[1]);
				auto lsb = std::dynamic_pointer_cast<NumberObject>(args_[2]);
				if (msb != nullptr && lsb != nullptr) {
					int64_t value = (static_cast<uint64_t>((uint32_t)msb->getValue()) << 32) | static_cast<uint64_t>((uint32_t)lsb->getValue());
					fmt::print("{}\n", value);
					fflush(stdout);
				}
			}
			static void println_String(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 2) {
					throw std::runtime_error("Not enough arguments for println");
				}
				auto clazz = std::dynamic_pointer_cast<ObjectClass>(args_[0]);
				if (!clazz || !clazz->isInstanceOf("java.io.PrintStream")) {
					throw std::runtime_error("First argument is not an instance of java.io.PrintStream");
				}
				// @todo handle stream kind (System.out, System.err, etc.) not always std::cout
				auto str = std::dynamic_pointer_cast<StringObject>(args_[1]);
				if (str != nullptr) {
					fmt::print("{}\n", str->str());
					fflush(stdout);
				}
			}
			static void print_String(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 2) {
					throw std::runtime_error("Not enough arguments for println");
				}
				auto clazz = std::dynamic_pointer_cast<ObjectClass>(args_[0]);
				if (!clazz || !clazz->isInstanceOf("java.io.PrintStream")) {
					throw std::runtime_error("First argument is not an instance of java.io.PrintStream");
				}
				// @todo handle stream kind (System.out, System.err, etc.) not always std::cout
				auto str = std::dynamic_pointer_cast<StringObject>(args_[1]);
				if (str != nullptr) {
					fmt::print("{}", str->str());
					fflush(stdout);
				}
			}
	};
}  // namespace

namespace java {
	namespace io {
		void PrintStream(::sandvik::ClassLoader& classLoader) {
			ClassBuilder _builder(classLoader, "java.io", "java.io.PrintStream");
			_builder.addVirtualMethod("println", "(I)V", 0, PrintStream::println_I);
			_builder.addVirtualMethod("println", "(J)V", 0, PrintStream::println_J);
			_builder.addVirtualMethod("println", "(Ljava/lang/String;)V", 0, PrintStream::println_String);
			_builder.addVirtualMethod("print", "(Ljava/lang/String;)V", 0, PrintStream::print_String);
			_builder.finalize();
		}
	}  // namespace io
}  // namespace java