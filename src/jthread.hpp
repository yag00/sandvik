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
#include <thread>
#include <vector>

#include "object.hpp"

namespace sandvik {
	class Vm;
	class Frame;
	class Method;
	class Interpreter;
	class ClassLoader;
	/** @brief Java thread representation */
	class JThread {
		public:
			/** @brief Constructs a new Java thread.
			 * @param vm_ Reference to the VM instance
			 * @param classloader_ Reference to the class loader
			 * @param name_ Name of the thread
			 */
			explicit JThread(Vm& vm_, ClassLoader& classloader_, const std::string& name_);
			/** @brief Constructs a new Java thread with an existing thread object.
			 * @param vm_ Reference to the VM instance
			 * @param classloader_ Reference to the class loader
			 * @param thread_ Shared pointer to the Java Thread object
			 */
			explicit JThread(Vm& vm_, ClassLoader& classloader_, ObjectRef thread_);
			~JThread();

			/** @brief Gets the name of the thread.
			 * @return Name of the thread
			 */
			inline std::string getName() const {
				return _name;
			}
			/** @brief Gets the thread ID.
			 * @return Thread ID
			 */
			inline std::thread::id getId() const {
				return _thread.get_id();
			}

			/** @brief Gets the VM instance.
			 * @return Reference to the VM instance
			 */
			Vm& vm() const;
			/** @brief Gets the class loader.
			 * @return Reference to the class loader
			 */
			ClassLoader& getClassLoader() const;
			/** @brief Checks if the thread has reached the end of execution.
			 * @return true if the thread has ended, false otherwise
			 */
			bool end() const;

			/** @brief Creates a new frame for the specified method.
			 * @param method_ Reference to the method
			 * @return Reference to the created frame
			 */
			Frame& newFrame(Method& method_);
			/** @brief Pops the current frame from the stack. */
			void popFrame();
			/** @brief Gets the current frame.
			 * @return Reference to the current frame
			 */
			Frame& currentFrame() const;
			/** @brief Gets the current stack depth.
			 * @return Current stack depth
			 */
			uint64_t stackDepth() const;

			/** @brief Runs the thread.
			 * @param wait_ If true, waits for the thread to finish
			 */
			void run(bool wait_ = false);
			/** @brief Joins the thread (waits for it to finish). */
			void join();

			/** @brief Gets the thread object.
			 * @return Shared pointer to the thread object
			 */
			ObjectRef getThreadObject() const;
			/** @brief Gets the return object of the thread.
			 * @return Shared pointer to the return object
			 */
			ObjectRef getReturnObject() const;
			/** @brief Gets the integer (32-bit) return value of the thread.
			 * @return Return value
			 */
			int32_t getReturnValue() const;
			/** @brief Gets the long (64-bit) return value of the thread.
			 * @return Return long value
			 */
			int64_t getReturnDoubleValue() const;
			/** @brief Sets the return object of the thread.
			 * @param ret_ Shared pointer to the return object
			 */
			void setReturnObject(ObjectRef ret_);
			/** @brief Sets the integer (32-bit) return value of the thread.
			 * @param ret_ Return value
			 */
			void setReturnValue(int32_t ret_);
			/** @brief Sets the long (64-bit) return value of the thread.
			 * @param ret_ Return long value
			 */
			void setReturnDoubleValue(int64_t ret_);

			/** @brief Checks if the thread is currently running.
			 * @return true if the thread is running, false otherwise
			 */
			bool isRunning() const;

		private:
			Vm& _vm;
			ClassLoader& _classloader;
			std::string _name;
			std::unique_ptr<Interpreter> _interpreter;

			std::vector<std::unique_ptr<Frame>> _stack;
			ObjectRef _objectReturn;
			ObjectRef _thisThread;

			std::thread _thread;
			std::atomic<bool> _isRunning{false};
	};
}  // namespace sandvik

#endif  // __JAVA_THREAD_HPP__