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

#ifndef __JVM_HPP__
#define __JVM_HPP__

#include <memory>
#include <string>
#include <vector>

namespace sandvik {
	class Class;
	class ClassLoader;
	class SharedLibrary;
	class NativeInterface;
	class Vm {
		public:
			explicit Vm();
			~Vm() = default;

			void loadRt(const std::string& path = "");
			void loadDex(const std::string& path);
			void loadApk(const std::string& path);
			void addClassPath(const std::string& classpath_);
			std::string getClassPath() const;

			void run();
			void run(const std::string& mainClass_, const std::vector<std::string>& args_);
			void stop();

			void loadLibrary(const std::string& libName_);
			void* findNativeSymbol(const std::string& symbolName_);
			ClassLoader& getClassLoader() const;
			NativeInterface* getJNIEnv() const;

		protected:
			void run(Class& clazz_, const std::vector<std::string>& args_);

		private:
			std::unique_ptr<ClassLoader> _classloader;
			std::vector<std::string> _ldpath;
			std::vector<std::unique_ptr<SharedLibrary>> _sharedlibs;
			std::unique_ptr<NativeInterface> _jnienv;
			bool _isPrimitiveClassInitialized = false;
	};
}  // namespace sandvik

#endif  // __JVM_HPP__