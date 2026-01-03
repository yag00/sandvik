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

#ifndef __MONITOR_HPP__
#define __MONITOR_HPP__

#include <condition_variable>
#include <mutex>
#include <thread>

namespace sandvik {
	/**
	 * @class Monitor
	 * @brief Java synchronization primitive.
	 */
	class Monitor {
		public:
			Monitor() = default;
			virtual ~Monitor() = default;

			Monitor(const Monitor&) = delete;
			Monitor& operator=(const Monitor&) = delete;
			Monitor(Monitor&& other) noexcept = delete;
			Monitor& operator=(Monitor&& other) noexcept = delete;

			/** @brief monitor enter */
			virtual void enter();
			/** @brief monitor exit */
			virtual void exit();
			/** @brief monitor check : block until the current thread either owns the monitor or the monitor is free.
			 *  We don't take ownership here; we only wait until it's safe for the caller to proceed.
			 */
			void check() const;

			/** @brief Causes the current thread to wait until either another thread invokes the notify methods
			 * @param timeout_ the maximum time to wait in milliseconds.
			 * @return true if notified before timeout, false if timed out.
			 */
			bool wait(uint64_t timeout_ = 0);
			/** @brief Wakes up a single thread that is waiting on this monitor.
			 *
			 * If any threads are waiting on this monitor, one of them
			 * is chosen to be awakened. */
			void notify();
			/** @brief Wakes up all threads that are waiting on this monitor. */
			void notifyAll();

		private:
			mutable std::mutex _mutex;
			std::condition_variable _condition;       // used for monitor-enter/exit coordination
			std::condition_variable _wait_condition;  // used for wait/notify
			std::thread::id _owner;                   // Tracks the thread that owns the monitor
			uint32_t _recursion = 0;
	};
}  // namespace sandvik

#endif  // __MONITOR_HPP__
