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
#include "jnihandlemap.hpp"

#include <fmt/format.h>

#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

jobject JNIHandleMap::toJObject(const std::shared_ptr<Object>& obj) {
	std::scoped_lock lock(_mutex);
	if (!obj) return nullptr;
	// Store in a map: raw pointer (jobject) -> shared_ptr<Object>
	jobject handle = reinterpret_cast<jobject>(obj.get());
	_table[handle] = obj;
	logger.fdebug("JNIHandleMap: Created handle {:#x} for object {}", (uintptr_t)handle, obj->toString());
	return handle;
}

std::shared_ptr<Object> JNIHandleMap::fromJObject(jobject handle) {
	std::scoped_lock lock(_mutex);
	if (!handle) return nullptr;
	return _table[handle];  // look up shared_ptr
}

void JNIHandleMap::release(jobject handle) {
	std::scoped_lock lock(_mutex);
	_table.erase(handle);
}
