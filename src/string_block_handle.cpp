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

#include "string_block_handle.hpp"

using namespace sandvik;

StringBlockRegistry& StringBlockRegistry::instance() {
	static StringBlockRegistry inst;
	return inst;
}

jlong StringBlockRegistry::create(jlong apkAssetsCookie_) {
	std::lock_guard<std::mutex> lock(_mutex);
	uint64_t id = _nextId++;
	auto handle = std::make_unique<StringBlockHandle>();
	handle->apkAssetsCookie = apkAssetsCookie_;
	handle->closed = false;
	_handles.emplace(id, std::move(handle));
	return static_cast<jlong>(id);
}

StringBlockHandle* StringBlockRegistry::get(jlong cookie_) {
	std::lock_guard<std::mutex> lock(_mutex);
	auto it = _handles.find(static_cast<uint64_t>(cookie_));
	if (it == _handles.end()) {
		return nullptr;
	}
	return it->second.get();
}

bool StringBlockRegistry::destroy(jlong cookie_) {
	std::lock_guard<std::mutex> lock(_mutex);
	return _handles.erase(static_cast<uint64_t>(cookie_)) > 0;
}