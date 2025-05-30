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

#include "sharedlibrary.hpp"

#include <dlfcn.h>

#include <fmt/format.h>

#include <filesystem>
#include <sstream>

#include "env_var.hpp"

using namespace sandvik;

SharedLibrary::SharedLibrary(const std::string& name_) : _path(name_), _handle(0) {
}

SharedLibrary::~SharedLibrary() {
	unload();
}

std::string SharedLibrary::getFullPath(const std::string& name_) {
	std::string ldLibraryPath = system::env::get("LD_LIBRARY_PATH");
	std::istringstream pathStream(ldLibraryPath);
	std::string path;

	while (std::getline(pathStream, path, ':')) {
		std::filesystem::path p(path);
		p /= name_;
		if (std::filesystem::exists(p)) {
			return p.string();
		}
	}

	std::filesystem::path p(name_);
	if (p.is_relative()) {
		p = std::filesystem::absolute(p);
	}
	if (std::filesystem::exists(p)) {
		return p.string();
	}

	return name_;
}

std::string SharedLibrary::getFullPath() const {
	return SharedLibrary::getFullPath(_path);
}

void SharedLibrary::load() {
	_handle = dlopen(_path.c_str(), RTLD_NOW | RTLD_LOCAL);
	if (!_handle) {
		throw std::runtime_error(fmt::format("Cannot open library {} : {}", _path, dlerror()));
	}
}

void SharedLibrary::unload() {
	if (_handle) {
		dlclose(_handle);  // Ignoring errors in destructor (optional)
		_handle = nullptr;
	}
}

bool SharedLibrary::isLoaded() const {
	return (_handle != 0);
}

void* SharedLibrary::getAddressOfSymbol(const std::string& name_) {
	return dlsym(_handle, name_.c_str());
}
