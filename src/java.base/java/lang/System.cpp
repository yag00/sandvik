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
#include "vm.hpp"

using namespace sandvik;

namespace {
	class System {
		public:
			void static loadLibrary(Vm& vm_, Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 1) {
					throw std::runtime_error("Not enough arguments for loadLibrary");
				}
				auto lib = std::dynamic_pointer_cast<StringObject>(args_[0]);
				if (!lib) {
					throw std::runtime_error("Argument must be an instance of java.lang.String");
				}
				auto libName = fmt::format("lib{}.so", lib->str());
				logger.info(fmt::format("Loading library: {}", libName));
				vm_.loadLibrary(libName);
			}
	};
}  // namespace

namespace java {
	namespace lang {
		void System(::sandvik::Vm& vm, ::sandvik::ClassLoader& classLoader) {
			ClassBuilder builder(classLoader, "java.lang", "java.lang.System");
			// @todo fix the static initialization of System class here we have dependency on PrintStream
			::sandvik::Class& printStreamClass = classLoader.getOrLoad("java.io.PrintStream");
			builder.addField("out", "Ljava/io/PrintStream;", true, Object::make(printStreamClass));
			builder.addMethod("loadLibrary", "(Ljava/lang/String;)V", ACCESS_FLAGS::ACC_STATIC,
			                  [&vm](Frame& frame_, std::vector<std::shared_ptr<::sandvik::Object>>& args_) { System::loadLibrary(vm, frame_, args_); });
			builder.finalize();
		}
	}  // namespace lang
}  // namespace java