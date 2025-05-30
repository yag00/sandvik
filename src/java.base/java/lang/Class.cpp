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

#include "class.hpp"

#include <fmt/format.h>

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

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
	class Clazz {
		public:
			static void getName(ClassLoader& classloader_, Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 1) {
					throw std::runtime_error("Not enough arguments for getName");
				}
				auto clazz = std::dynamic_pointer_cast<ConstClassObject>(args_[0]);
				if (!clazz || !clazz->isInstanceOf("java.lang.Class")) {
					throw std::runtime_error("First argument is not an instance of java.lang.Class");
				}
				frame_.setReturnObject(Object::make(classloader_, clazz->getClassType().getFullname()));
			}
	};
}  // namespace

namespace java {
	namespace lang {
		void Class(::sandvik::ClassLoader& classLoader) {
			ClassBuilder builder(classLoader, "java.lang", "java.lang.Class");
			builder.addVirtualMethod("<init>", "()V", 0, [](Frame&, std::vector<std::shared_ptr<::sandvik::Object>>&) {});
			builder.addVirtualMethod(
			    "getName", "()Ljava/lang/String;", 0,
			    [&classLoader](Frame& frame_, std::vector<std::shared_ptr<::sandvik::Object>>& args_) { Clazz::getName(classLoader, frame_, args_); });
			builder.finalize();
		}
	}  // namespace lang
}  // namespace java