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

#include "class.hpp"
#include "dex.hpp"
#include "exceptions.hpp"
#include "system/logger.hpp"
#include "system/zip.hpp"

extern "C" {
	extern const unsigned char _binary_sanddirt_dex_jar_start[];
	extern const unsigned char _binary_sanddirt_dex_jar_end[];
	extern const size_t _binary_sanddirt_dex_jar_size;
}

using namespace sandvik;

/** Constructor: Loads the JAR file */
void rtld::load(const std::string& path_, std::vector<std::unique_ptr<Dex>>& dexs_) {
	auto zip = std::make_unique<ZipReader>();
	if (path_.empty()) {
		size_t size = (size_t)&_binary_sanddirt_dex_jar_size;
		// paranoia check
		size_t size2 = (uintptr_t)_binary_sanddirt_dex_jar_end - (uintptr_t)_binary_sanddirt_dex_jar_start;
		if (size != size2) {
			throw VmException("Internal error: embedded RT size mismatch {} != {}", size, size2);
		}
		zip->open((uint8_t*)_binary_sanddirt_dex_jar_start, size);
	} else {
		if (!ZipReader::isValidArchive(path_)) {
			throw VmException("Invalid RT file: {}", path_);
		}
		zip->open(path_);
	}

	// load all *.dex files
	for (const auto& file : zip->getList()) {
		if (file.size() >= 4 && file.substr(file.size() - 4) == ".dex") {
			uint64_t size = 0;
			char* buffer = zip->extractToMemory(file, size);
			if (buffer == nullptr) {
				throw VmException("Failed to extract {}", file);
			}
			std::vector<uint8_t> dexBuffer(buffer, buffer + size);
			free(buffer);
			// logger.fdebug("RT DEX loaded: {}", file));
			dexs_.push_back(std::make_unique<Dex>(dexBuffer, file));
		}
	}
	zip->close();
}