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

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace sandvik {
	class Class;
	class ClassLoader;
	class SharedLibrary;
	class NativeInterface;
	class JThread;
	class Object;
	class Vm {
		public:
			explicit Vm();
			~Vm();

			void loadRt(const std::string& path = "");
			void loadDex(const std::string& path);
			void loadApk(const std::string& path);
			void addClassPath(const std::string& classpath_);
			std::string getClassPath() const;

			std::string getProperty(const std::string& name_) const;

			void run();
			void run(const std::string& mainClass_, const std::vector<std::string>& args_);
			void stop();
			inline bool isRunning() const volatile {
				return _isRunning;
			}

			JThread& newThread(const std::string& name_);
			JThread& newThread(std::shared_ptr<Object> thread_);
			JThread& getThread(const std::string& name_);
			JThread& currentThread() const;
			void deleteThread(const std::string& name_);

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
			std::vector<std::unique_ptr<JThread>> _threads;
			std::unique_ptr<NativeInterface> _jnienv;
			std::map<std::string, std::string> _properties;
			bool _isPrimitiveClassInitialized = false;
			volatile bool _isRunning = false;
	};
}  // namespace sandvik

#endif  // __JVM_HPP__