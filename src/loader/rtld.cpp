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

#include <string>
#include <vector>

#include "fmt/format.h"

#include "class.hpp"
#include "dex.hpp"
#include "system/logger.hpp"
#include "system/zip.hpp"

using namespace sandvik;

/** Constructor: Loads the JAR file */
void rtld::load(const std::string& path_, std::vector<std::unique_ptr<Dex>>& dexs_) {
	if (!ZipReader::isValidArchive(path_)) {
		throw std::runtime_error(fmt::format("Invalid RT file: {}", path_));
	}

	auto zip = std::make_unique<ZipReader>();
	zip->open(path_);
	// load all *.dex files
	for (const auto& file : zip->getList()) {
		if (file.size() >= 4 && file.substr(file.size() - 4) == ".dex") {
			uint64_t size = 0;
			char* buffer = zip->extractToMemory(file, size);
			if (buffer == nullptr) {
				throw std::runtime_error(fmt::format("Failed to extract {}", file));
			}
			std::vector<uint8_t> dexBuffer(buffer, buffer + size);
			free(buffer);
			// logger.debug(fmt::format("RT DEX loaded: {}", file));
			dexs_.push_back(std::make_unique<Dex>(dexBuffer, file));
		}
	}
	zip->close();
}