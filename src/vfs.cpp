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

#include "vfs.hpp"

#include "system/logger.hpp"

using namespace sandvik;

void VFS::setAndroidRoot(const std::string& path) {
	androidRoot = std::filesystem::absolute(path).string();
}

std::string VFS::resolve(const std::string& virtualPath) {
	if (virtualPath.empty()) return virtualPath;

	// If the virtual path is already an absolute path in the host cache, return it as is.
	if (virtualPath.rfind(hostCacheRoot, 0) == 0) {
		return virtualPath;
	}

	if (virtualPath[0] == '/') {
		// It the file does not exist in the host filesystem, resolve to virtual path in the Android Root directory.
		if (!std::filesystem::exists(virtualPath)) {
			logger.fdebug("VFS::resolve('{}') -> '{}'", virtualPath, androidRoot + virtualPath);
			return androidRoot + virtualPath;
		}
	}
	return virtualPath;
}
