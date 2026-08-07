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

#include "rtld.hpp"

#include <filesystem>
#include <string>
#include <vector>

#include "class.hpp"
#include "dex.hpp"
#include "exceptions.hpp"
#include "system/logger.hpp"
#include "system/zip.hpp"

using namespace sandvik;

/** Constructor: Loads the JAR file */
void rtld::loadJar(const std::string& path_, std::vector<std::unique_ptr<Dex>>& dexs_) {
	auto zip = std::make_unique<ZipReader>();
	if (path_.empty()) {
		throw VmException("Invalid RT file: empty path");
	} else {
		if (!ZipReader::isValidArchive(path_)) {
			throw VmException("Invalid RT file: {}", path_);
		}
		zip->open(path_);
	}
	logger.fdebug("Loading JAR: {}", path_);

	// load all *.dex files
	for (const auto& file : zip->getList()) {
		if (file.size() >= 4 && file.ends_with(".dex")) {
			uint64_t size = 0;
			auto buffer = zip->extractToMemory(file, size);
			if (!buffer) {
				throw VmException("Failed to extract {}", file);
			}
			std::vector<uint8_t> dexBuffer(buffer.get(), buffer.get() + size);
			dexs_.push_back(std::make_unique<Dex>(dexBuffer, path_.empty() ? "<sandvik>" : path_));
		}
	}
	zip->close();
}

std::vector<std::string> rtld::resolveJarOrder(const std::string& jarsDir_) {
	std::vector<std::string> jarFiles;
	std::string orderFile = jarsDir_ + "/order.txt";
	if (std::filesystem::exists(orderFile)) {
		std::ifstream infile(orderFile);
		std::string line;
		while (std::getline(infile, line)) {
			if (!line.empty()) {
				jarFiles.push_back(jarsDir_ + "/" + line);
			}
		}
	} else {
		logger.fdebug("No order.txt found in {}, loading JARs in alphabetical order", jarsDir_);
		for (const auto& entry : std::filesystem::directory_iterator(jarsDir_)) {
			if (entry.path().extension() == ".jar") {
				jarFiles.push_back(entry.path().string());
			}
		}
		std::sort(jarFiles.begin(), jarFiles.end());
	}
	return jarFiles;
}

void rtld::load(const std::string& jarsDir_, std::vector<std::unique_ptr<Dex>>& dexs_, std::vector<std::string>* loadedJarPaths_) {
	auto jarFiles = resolveJarOrder(jarsDir_);
	for (const auto& jarFile : jarFiles) {
		loadJar(jarFile, dexs_);
		if (loadedJarPaths_) {
			loadedJarPaths_->push_back(jarFile);
		}
	}
}

std::optional<std::vector<uint8_t>> sandvik::rtld::findResourceInJar(const std::string& jarPath_, const std::string& name_) {
	try {
		ZipReader reader;
		reader.open(jarPath_);
		uint64_t size = 0;
		std::unique_ptr<char[]> data = reader.extractToMemory(name_, size);
		reader.close();
		if (data && size > 0) {
			return std::vector<uint8_t>(reinterpret_cast<uint8_t*>(data.get()), reinterpret_cast<uint8_t*>(data.get()) + size);
		}
	} catch (const std::exception&) {
		// absent de ce jar, ou jar illisible
	}
	return std::nullopt;
}