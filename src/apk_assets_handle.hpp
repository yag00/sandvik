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

#ifndef __APK_ASSETS_HANDLE_HPP__
#define __APK_ASSETS_HANDLE_HPP__

#include <jni/jni.h>

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace sandvik {

	/**
	 * @struct ApkAssetsHandle
	 * @brief Placeholder state for an ApkAssets "load" while sandvik does not
	 *        yet implement real .arsc (resource table) parsing.
	 *
	 * This intentionally does not try to look like a real resource table.
	 * It exists so native methods that receive a jlong cookie back from Java
	 * (nativeGetStringBlock, nativeClose, etc.) have something valid to look
	 * up, log against, and eventually extend once real resource parsing is
	 * implemented.
	 */
	struct ApkAssetsHandle {
			/** @brief Path this handle was created for (for logging/debugging). */
			std::string path;
			/** @brief Set once nativeClose has been called on this handle. */
			bool closed = false;
	};

	/**
	 * @class ApkAssetsRegistry
	 * @brief Owns all ApkAssetsHandle instances and hands out opaque
	 *        integer ids as jlong cookies, instead of raw pointers.
	 *
	 * Rationale: casting a raw C++ pointer to jlong and back is unverifiable
	 * on the way back in — a corrupted or stale cookie coming from a VM or
	 * JNI marshaling bug would silently dereference garbage memory. An
	 * id-indexed table lets every lookup be validated and logged instead of
	 * crashing opaquely.
	 *
	 * Thread-safety: guarded by an internal mutex, since the zygote forks
	 * processes that may end up touching AssetManager instances from more
	 * than one thread.
	 */
	class ApkAssetsRegistry {
		public:
			/** @brief Returns the process-wide registry instance. */
			static ApkAssetsRegistry& instance();

			/**
			 * @brief Creates a new handle for the given path.
			 * @param path_ Path the handle was "loaded" from.
			 * @return Opaque jlong cookie identifying the handle.
			 */
			jlong create(const std::string& path_);

			/**
			 * @brief Looks up a handle by cookie.
			 * @param cookie_ Cookie previously returned by create().
			 * @return Pointer to the handle, or nullptr if the cookie is unknown.
			 */
			ApkAssetsHandle* get(jlong cookie_);

			/**
			 * @brief Removes and destroys the handle associated with a cookie.
			 * @param cookie_ Cookie previously returned by create().
			 * @return true if a handle was found and destroyed, false otherwise.
			 */
			bool destroy(jlong cookie_);

			/** @brief Number of currently live (non-destroyed) handles. Mostly for debugging/tests. */
			size_t size() const;

		private:
			ApkAssetsRegistry() = default;

			mutable std::mutex _mutex;
			std::unordered_map<uint64_t, std::unique_ptr<ApkAssetsHandle>> _handles;
			uint64_t _nextId = 1;
	};

}  // namespace sandvik

#endif  // __APK_ASSETS_HANDLE_HPP__