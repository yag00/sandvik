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

#ifndef __JAVA_THREAD_HPP__
#define __JAVA_THREAD_HPP__

#include <memory>
#include <string>
#include <vector>

namespace sandvik {
	class Vm;
	class Object;
	class Frame;
	class Method;
	class Interpreter;
	class ClassLoader;
	class JThread {
		public:
			explicit JThread(Vm& vm_, ClassLoader& classloader_, const std::string& name_);
			~JThread() = default;

			Vm& vm() const;
			ClassLoader& getClassLoader() const;
			bool end() const;

			Frame& newFrame(Method& method_);
			void popFrame();
			Frame& currentFrame() const;
			uint64_t stackDepth() const;

			void execute();

		private:
			Vm& _vm;
			ClassLoader& _classloader;
			std::string _name;
			std::unique_ptr<Interpreter> _interpreter;

			std::vector<std::unique_ptr<Frame>> _stack;
	};
}  // namespace sandvik

#endif  // __JAVA_THREAD_HPP__