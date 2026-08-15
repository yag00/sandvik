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

#ifndef __VFS_HPP__
#define __VFS_HPP__

#include <filesystem>
#include <string>

namespace sandvik {
	/**
	 * @brief Virtual File System (VFS) for the Sandvik project.
	 */
	class VFS {
		private:
			static inline std::string androidRoot = "./android_root";

			static inline std::string hostCacheRoot = "/tmp/sandvik-res-cache";

		public:
			/**
			 * @brief Gets the host cache root directory for the virtual file system.
			 *
			 * @return The path to the host cache root directory.
			 */
			static std::string getHostCacheRoot() {
				return hostCacheRoot;
			}

			/**
			 * @brief Sets the Android root directory for the virtual file system.
			 *
			 * @param path The path to the Android root directory.
			 */
			static void setAndroidRoot(const std::string& path);

			/**
			 * @brief Resolves a virtual Android file path to the corresponding host file path.
			 *
			 * @param virtualPath The virtual Android file path (e.g., "/misc/zoneinfo/...").
			 * @return The corresponding host file path (e.g., "/home/user/emulator/android_root/misc/zoneinfo/...").
			 */
			static std::string resolve(const std::string& virtualPath);
	};
}  // namespace sandvik

#endif  // __VFS_HPP__