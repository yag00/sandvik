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

#ifndef __STRING_BLOCK_HANDLE_HPP__
#define __STRING_BLOCK_HANDLE_HPP__

#include <jni/jni.h>

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace sandvik {

	/**
	 * @struct StringBlockHandle
	 * @brief Placeholder state for a StringBlock while sandvik does not yet
	 *        parse real .arsc string pools.
	 *
	 * Tracks which ApkAssets cookie it was created from, for logging/debugging.
	 * Holds no real string data yet; string lookups against this handle should
	 * be treated by callers as "not found" until real .arsc parsing exists.
	 */
	struct StringBlockHandle {
			/** @brief Cookie of the ApkAssets this string block was requested from. */
			jlong apkAssetsCookie = 0;
			/** @brief Set once nativeClose (or equivalent) has been called. */
			bool closed = false;
	};

	/**
	 * @class StringBlockRegistry
	 * @brief Owns all StringBlockHandle instances, handing out opaque integer
	 *        ids as jlong cookies instead of raw pointers. Same rationale as
	 *        StubApkAssetsRegistry: avoids dereferencing a raw pointer cast
	 *        from a jlong that may be corrupted by an unrelated VM bug.
	 */
	class StringBlockRegistry {
		public:
			/** @brief Returns the process-wide registry instance. */
			static StringBlockRegistry& instance();

			/**
			 * @brief Creates a new string block handle for a given ApkAssets cookie.
			 * @param apkAssetsCookie_ Cookie previously returned by ApkAssets.nativeLoad.
			 * @return Opaque jlong cookie identifying the new string block handle.
			 */
			jlong create(jlong apkAssetsCookie_);

			/**
			 * @brief Looks up a string block handle by cookie.
			 * @param cookie_ Cookie previously returned by create().
			 * @return Pointer to the handle, or nullptr if the cookie is unknown.
			 */
			StringBlockHandle* get(jlong cookie_);

			/**
			 * @brief Removes and destroys the handle associated with a cookie.
			 * @param cookie_ Cookie previously returned by create().
			 * @return true if a handle was found and destroyed, false otherwise.
			 */
			bool destroy(jlong cookie_);

		private:
			StringBlockRegistry() = default;

			mutable std::mutex _mutex;
			std::unordered_map<uint64_t, std::unique_ptr<StringBlockHandle>> _handles;
			uint64_t _nextId = 1;
	};

}  // namespace sandvik

#endif  // __STRING_BLOCK_HANDLE_HPP__