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
