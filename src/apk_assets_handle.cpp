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

#include "apk_assets_handle.hpp"

using namespace sandvik;

ApkAssetsRegistry& ApkAssetsRegistry::instance() {
	static ApkAssetsRegistry inst;
	return inst;
}

jlong ApkAssetsRegistry::create(const std::string& path_) {
	std::lock_guard<std::mutex> lock(_mutex);
	uint64_t id = _nextId++;
	auto handle = std::make_unique<ApkAssetsHandle>();
	handle->path = path_;
	handle->closed = false;
	_handles.emplace(id, std::move(handle));
	return static_cast<jlong>(id);
}

ApkAssetsHandle* ApkAssetsRegistry::get(jlong cookie_) {
	std::lock_guard<std::mutex> lock(_mutex);
	auto it = _handles.find(static_cast<uint64_t>(cookie_));
	if (it == _handles.end()) {
		return nullptr;
	}
	return it->second.get();
}

bool ApkAssetsRegistry::destroy(jlong cookie_) {
	std::lock_guard<std::mutex> lock(_mutex);
	return _handles.erase(static_cast<uint64_t>(cookie_)) > 0;
}

size_t ApkAssetsRegistry::size() const {
	std::lock_guard<std::mutex> lock(_mutex);
	return _handles.size();
}