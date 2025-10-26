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

			/** monitor enter */
			virtual void enter();
			/** monitor exit */
			virtual void exit();
			/** monitor check : block until the current thread either owns the monitor or the monitor is free.
			 *  We don't take ownership here; we only wait until it's safe for the caller to proceed.
			 */
			void check() const;

		private:
			mutable std::mutex _mutex;
			std::condition_variable _condition;
			std::thread::id _owner;  // Tracks the thread that owns the monitor
	};
}  // namespace sandvik

#endif  // __MONITOR_HPP__
