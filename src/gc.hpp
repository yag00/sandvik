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

#include <memory>
#include <system/singleton.hpp>
#include <vector>

namespace sandvik {
	class Object;
	/** @brief Garbage Collector singleton class.
	 */
	class GC : public Singleton<GC> {
		public:
		public:
			/** release all tracked objects */
			void release();
			/** perform garbage collection */
			void collect();

			/** track allocated object
			 * @param obj_ object to track (take ownership)
			 */
			void track(std::unique_ptr<Object> obj_);

		private:
			// tracked objects
			std::vector<std::unique_ptr<Object>> _objects;
	};
}  // namespace sandvik

#endif  // __GARBAGE_COLLECTOR_HPP__