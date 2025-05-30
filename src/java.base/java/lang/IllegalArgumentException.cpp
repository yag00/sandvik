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
	class IllegalArgumentException {
		public:
			static void init(ClassLoader& classloader_, Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 2) {
					throw std::runtime_error("Not enough arguments for IllegalArgumentException.<init>");
				}
				auto clazz = std::dynamic_pointer_cast<ObjectClass>(args_[0]);
				if (!clazz || !clazz->isInstanceOf("java.lang.IllegalArgumentException")) {
					throw std::runtime_error("First argument is not an instance of java.lang.IllegalArgumentException");
				}
				auto message = std::dynamic_pointer_cast<StringObject>(args_[1]);
				if (!message) {
					throw std::runtime_error("Second argument is not an instance of java.lang.String");
				}
				clazz->setObjectData(std::make_shared<std::string>(message->str()));
			}
			static void getMessage(ClassLoader& classloader_, Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 1) {
					throw std::runtime_error("Not enough arguments for IllegalArgumentException.getMessage");
				}
				auto clazz = std::dynamic_pointer_cast<ObjectClass>(args_[0]);
				if (!clazz || !clazz->isInstanceOf("java.lang.IllegalArgumentException")) {
					throw std::runtime_error("First argument is not an instance of java.lang.IllegalArgumentException");
				}
				auto message = clazz->getObjectData<std::string>();
				if (!message) {
					throw std::runtime_error("No message set for IllegalArgumentException");
				}
				frame_.setReturnObject(Object::make(classloader_, *message));
			}
	};
}  // namespace

namespace java {
	namespace lang {
		void IllegalArgumentException(::sandvik::ClassLoader& classLoader) {
			ClassBuilder builder(classLoader, "java.lang", "java.lang.IllegalArgumentException");
			builder.addVirtualMethod("<init>", "(Ljava/lang/String;)V", 0,
			                         [&classLoader](Frame& frame_, std::vector<std::shared_ptr<::sandvik::Object>>& args_) {
				                         IllegalArgumentException::init(classLoader, frame_, args_);
			                         });
			builder.addVirtualMethod("getMessage", "()Ljava/lang/String;", 0,
			                         [&classLoader](Frame& frame_, std::vector<std::shared_ptr<::sandvik::Object>>& args_) {
				                         IllegalArgumentException::getMessage(classLoader, frame_, args_);
			                         });
			builder.finalize();
		}
	}  // namespace lang
}  // namespace java