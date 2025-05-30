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

#ifndef __CLASSBUILDER_HPP__
#define __CLASSBUILDER_HPP__

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace sandvik {
	class ClassLoader;
	class Frame;
	class Object;
	class Class;
	class ClassBuilder {
		public:
			ClassBuilder(ClassLoader& classLoader, const std::string& packageName, const std::string& className);

			void addVirtualMethod(const std::string& name_, const std::string& signature_, uint64_t flags_,
			                      std::function<void(Frame&, std::vector<std::shared_ptr<Object>>&)> function_);
			void addMethod(const std::string& name_, const std::string& signature_, uint64_t flags,
			               std::function<void(Frame&, std::vector<std::shared_ptr<Object>>&)> function_);

			void addField(const std::string& name_, const std::string& type_, bool isStatic_, std::shared_ptr<Object> value = nullptr);

			void setSuperClass(const std::string& superClassName_);
			void setInterface();

			void finalize();

		private:
			ClassLoader& _classLoader;
			std::unique_ptr<Class> _class;
	};

}  // namespace sandvik

#endif  // __CLASSBUILDER_HPP__