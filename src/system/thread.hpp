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

#ifndef __SYSTEM_THREAD_HPP__
#define __SYSTEM_THREAD_HPP__

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

namespace sandvik {
	/** @brief Thread */
	class Thread {
		public:
			/** @brief Thread states */
			enum class ThreadState { NotStarted, Running, SuspendedRequested, Suspended, Stopped };
			/** @brief Constructor */
			explicit Thread(const std::string& name_);
			/** @brief Destructor */
			virtual ~Thread();

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

			/** @brief Runs the thread.
			 * @param wait_ If true, waits for the thread to finish
			 */
			void run(bool wait_ = false);
			/** @brief Joins the thread (waits for it to finish). */
			virtual void join();

			/** @brief Checks if the thread is currently running.
			 * @return true if the thread is running, false otherwise
			 */
			bool isRunning() const;
			/** @brief Checks if the thread is currently running.
			 * @return the thread state
			 */
			ThreadState getState() const;

			/** @brief Suspends the thread execution. */
			void suspend();
			/** @brief Resumes the thread execution. */
			void resume();
			/** @brief Stop the thread execution. */
			void stop();

		protected:
			/** @brief thread loop function of the thread implemented by subclass. */
			virtual void loop() = 0;
			/** @brief thread loop end condition. */
			virtual bool done() = 0;
			/** @brief hook called when run() is about to start a new thread. */
			virtual void onStart() {
			}

		private:
			std::string _name;
			std::thread _thread;
			std::atomic<ThreadState> _state{ThreadState::NotStarted};
			std::atomic<ThreadState> _actualState{ThreadState::NotStarted};

			std::mutex _mtx;
			std::condition_variable _cv;
	};
}  // namespace sandvik

#endif  // __SYSTEM_THREAD_HPP__
