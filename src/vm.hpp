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

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

/** @brief sandvik : project namespace */
namespace sandvik {
	class Class;
	class ClassLoader;
	class SharedLibrary;
	class NativeInterface;
	class JThread;
	class Object;
	/** @class Vm
	 *  @brief Dalvik Java Virtual Machine implementation.
	 *
	 *  The Vm class represents the base class of the Dalvik Java Virtual Machine.
	 */
	class Vm {
		public:
			explicit Vm();
			~Vm();

			/** Load runtime libraries
			 * @param path_ Path to the runtime libraries
			 */
			void loadRt(const std::string& path_ = "");
			/** Load DEX files
			 * @param path_ Path to the DEX file
			 */
			void loadDex(const std::string& path_);
			/** Load APK files
			 * @param path_ Path to the APK file
			 */
			void loadApk(const std::string& path_);
			/** Add a class path
			 * @param classpath_ Class path to add
			 */
			void addClassPath(const std::string& classpath_);
			/** Get the current class path
			 * @return Current class path
			 */
			std::string getClassPath() const;
			/** Set a system property
			 * @param name_ Property name
			 * @param value_ Property value
			 */
			void setProperty(const std::string& name_, const std::string& value_);
			/** Get a system property
			 * @param name_ Property name
			 * @return Property value
			 */
			std::string getProperty(const std::string& name_) const;

			/** Run the virtual machine */
			void run();
			/** Run the virtual machine with a main class and arguments
			 * @param mainClass_ Main class to run
			 * @param args_ Arguments to pass to the main class
			 */
			void run(const std::string& mainClass_, const std::vector<std::string>& args_);
			/** Stop the virtual machine */
			void stop();
			/** Check if the virtual machine is running
			 * @return true if the VM is running, false otherwise
			 */
			inline bool isRunning() const {
				return _isRunning.load();
			}

			/** Create a new thread
			 * @param name_ Name of the thread
			 * @return Reference to the created thread
			 */
			JThread& newThread(const std::string& name_);
			/** Create a new thread with an existing thread object
			 * @param thread_ Java Thread Object
			 * @return Reference to the created thread
			 */
			JThread& newThread(std::shared_ptr<Object> thread_);
			/** Get a thread by name
			 * @param name_ Name of the thread
			 * @return Reference to the thread
			 */
			JThread& getThread(const std::string& name_);
			/** Get the current thread
			 * @return Reference to the current thread
			 */
			JThread& currentThread() const;
			/** Delete a thread by name
			 * @param name_ Name of the thread
			 */
			void deleteThread(const std::string& name_);

			/** Load a shared library
			 * @param libName_ Name of the library
			 */
			void loadLibrary(const std::string& libName_);
			/** Find a native symbol in the loaded libraries
			 * @param symbolName_ Name of the symbol
			 * @return Pointer to the symbol, or nullptr if not found
			 */
			void* findNativeSymbol(const std::string& symbolName_);
			/** Get the class loader
			 * @return Reference to the class loader
			 */
			ClassLoader& getClassLoader() const;
			/** Get the JNI environment
			 * @return Pointer to the JNI environment
			 */
			NativeInterface* getJNIEnv() const;

		protected:
			/** Run main method of given class with given arguments
			 * @param clazz_ Class to run
			 * @param args_ Arguments to pass to the class
			 */
			void run(Class& clazz_, const std::vector<std::string>& args_);

		private:
			std::unique_ptr<ClassLoader> _classloader;
			std::vector<std::string> _ldpath;
			std::vector<std::unique_ptr<SharedLibrary>> _sharedlibs;

			std::vector<std::unique_ptr<JThread>> _threads;

			std::unique_ptr<NativeInterface> _jnienv;
			std::map<std::string, std::string, std::less<>> _properties;
			bool _isPrimitiveClassInitialized = false;
			std::atomic<bool> _isRunning{false};

			mutable std::mutex _mutex;
	};
}  // namespace sandvik

#endif  // __JVM_HPP__