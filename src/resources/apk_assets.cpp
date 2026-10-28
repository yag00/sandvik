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

#include "resources/apk_assets.hpp"

#include <fstream>
#include <iterator>

#include "system/logger.hpp"
#include "system/zip.hpp"

using namespace sandvik::resources;

ApkAssets::ApkAssets(std::string assetPath_) : _assetPath(std::move(assetPath_)) {
}

ApkAssets::~ApkAssets() {
	if (_zip) {
		_zip->close();
	}
}

std::shared_ptr<ApkAssets> ApkAssets::load(const std::string& hostPath_, Format format_, const std::string& assetPath_) {
	std::shared_ptr<ApkAssets> assets(new ApkAssets(assetPath_));
	switch (format_) {
		case Format::APK: {
			if (!ZipReader::isValidArchive(hostPath_)) {
				throw VmException("resources: {} is not an APK", hostPath_);
			}
			assets->_zip = std::make_unique<ZipReader>();
			assets->_zip->open(hostPath_);
			// an APK without resources (code only) is valid
			if (auto table = assets->readFile("resources.arsc")) {
				assets->_tableData = std::make_shared<std::vector<uint8_t>>(std::move(*table));
			}
			break;
		}
		case Format::ARSC: {
			std::ifstream file(hostPath_, std::ios::binary);
			if (!file) {
				throw VmException("resources: can't open {}", hostPath_);
			}
			assets->_tableData = std::make_shared<std::vector<uint8_t>>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
			break;
		}
		default:
			throw VmException("resources: unsupported ApkAssets format {} for {}", static_cast<int>(format_), hostPath_);
	}
	if (assets->_tableData) {
		assets->_table = std::make_unique<ResourceTable>(ByteView(assets->_tableData->data(), assets->_tableData->size()), assets->_tableData);
	}
	logger.fdebug("resources: loaded {} ({} package(s))", assetPath_, assets->_table ? assets->_table->getPackages().size() : 0);
	return assets;
}

std::shared_ptr<ApkAssets> ApkAssets::loadEmpty(const std::string& assetPath_) {
	return std::shared_ptr<ApkAssets>(new ApkAssets(assetPath_));
}

std::optional<std::vector<uint8_t>> ApkAssets::readFile(const std::string& name_) const {
	if (!_zip) {
		return std::nullopt;
	}
	std::lock_guard lock(_zipMutex);
	try {
		uint64_t size = 0;
		auto data = _zip->extractToMemory(name_, size);
		return std::vector<uint8_t>(data.get(), data.get() + size);
	} catch (const std::exception&) {
		return std::nullopt;
	}
}
