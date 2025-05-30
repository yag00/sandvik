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


#include "object.hpp"

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
#include "system/logger.hpp"

using namespace sandvik;

namespace {
	class Object {
		public:
	};
}  // namespace

namespace java {
	namespace lang {
		void Object(::sandvik::ClassLoader& classLoader) {
			ClassBuilder builder(classLoader, "java.lang", "java.lang.Object");
			builder.addVirtualMethod("<init>", "()V", 0, [](Frame&, std::vector<std::shared_ptr<::sandvik::Object>>&) {});
			builder.finalize();
		}
	}  // namespace lang
}  // namespace java