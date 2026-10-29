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

#ifndef __RESOURCES_HANDLE_REGISTRY_HPP__
#define __RESOURCES_HANDLE_REGISTRY_HPP__

#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace sandvik::resources {
	/**
	 * @class HandleRegistry
	 * @brief Native objects handed to Java as opaque long handles (ApkAssets.mNativePtr, AssetManager.mObject...).
	 *
	 * Java gets an id instead of a raw pointer: a stale or corrupted handle coming back from Java is detected
	 * (unknown id) instead of dereferencing garbage memory. Ids are never reused.
	 */
	template <typename T>
	class HandleRegistry {
		public:
			static HandleRegistry& instance() {
				static HandleRegistry registry;
				return registry;
			}

			int64_t add(std::shared_ptr<T> object_) {
				std::lock_guard lock(_mutex);
				const int64_t handle = _nextHandle++;
				_objects.emplace(handle, std::move(object_));
				return handle;
			}
			/** @brief Object of a handle, nullptr if the handle is unknown */
			std::shared_ptr<T> get(int64_t handle_) const {
				std::lock_guard lock(_mutex);
				auto it = _objects.find(handle_);
				return it == _objects.end() ? nullptr : it->second;
			}
			bool remove(int64_t handle_) {
				std::lock_guard lock(_mutex);
				return _objects.erase(handle_) > 0;
			}
			size_t size() const {
				std::lock_guard lock(_mutex);
				return _objects.size();
			}

		private:
			HandleRegistry() = default;
			mutable std::mutex _mutex;
			std::unordered_map<int64_t, std::shared_ptr<T>> _objects;
			int64_t _nextHandle = 1;
	};
}  // namespace sandvik::resources

#endif  // __RESOURCES_HANDLE_REGISTRY_HPP__
