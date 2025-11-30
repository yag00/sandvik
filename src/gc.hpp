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

#ifndef __GARBAGE_COLLECTOR_HPP__
#define __GARBAGE_COLLECTOR_HPP__

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "system/singleton.hpp"
#include "system/thread.hpp"

namespace sandvik {
	class Vm;
	class Object;
	/** @brief Garbage Collector singleton class.
	 */
	class GC : public Thread, public Singleton<GC> {
		public:
			GC();
			~GC() override;
			/** Add Vm to be managed by the GC
			 * @param vm_ Vm to manage
			 */
			void manageVm(Vm* vm_);
			/** Unmanage Vm from the GC
			 * @param vm_ Vm to unmanage
			 */
			void unmanageVm(Vm* vm_);
			/** Get the number of tracked objects
			 * @return number of tracked objects
			 */
			uint64_t getTrackedObjectCount() const;

			/** Get the number of garbage collection cycles
			 * @return cycles
			 */
			uint64_t getGcCycles() const;

			/** Get the object count limit
			 * @return object count limit
			 */
			uint64_t getLimit() const;

			/** Set the object count limit
			 * @param limit_ object count limit
			 */
			void setLimit(uint64_t limit_);

			/** release all tracked objects */
			void release();
			/** request garbage collection */
			void requestCollect();

			/** track allocated object
			 * @param obj_ object to track (take ownership)
			 */
			void track(std::unique_ptr<Object> obj_);

		protected:
			/** @brief thread loop function of the thread implemented by subclass. */
			void loop() override;
			/** @brief thread loop end condition. */
			bool done() override;
			/** @brief hook called when run() is about to start a new thread. */
			void onStart() override;

		private:
			/** perform garbage collection */
			void collect();

			// tracked objects
			std::vector<std::unique_ptr<Object>> _objects;
			// Vms
			std::vector<Vm*> _vms;

			// object count limit before triggering GC
			uint64_t _limit = 100000;
			std::mutex _mtx;
			std::condition_variable _cv;
			std::atomic<bool> _gcRequested{false};
			std::atomic<bool> _done{false};
			std::atomic<uint64_t> _cycles{0};
	};
}  // namespace sandvik

#endif  // __GARBAGE_COLLECTOR_HPP__