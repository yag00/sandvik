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

#ifndef __RESOURCES_APK_ASSETS_HPP__
#define __RESOURCES_APK_ASSETS_HPP__

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include "resources/resource_table.hpp"

namespace sandvik {
	class ZipReader;
}

namespace sandvik::resources {
	/**
	 * @class ApkAssets
	 * @brief A loaded resources container (androidfw ApkAssets): an APK or a bare resources.arsc.
	 */
	class ApkAssets {
		public:
			/** @brief ApkAssets.FORMAT_* */
			enum class Format { APK = 0, IDMAP = 1, ARSC = 2, DIRECTORY = 3 };

			~ApkAssets();
			ApkAssets(const ApkAssets&) = delete;
			ApkAssets& operator=(const ApkAssets&) = delete;

			/** @brief Loads an APK or an arsc file from the host filesystem.
			 * @throw VmException if the file cannot be read or parsed */
			static std::shared_ptr<ApkAssets> load(const std::string& hostPath_, Format format_, const std::string& assetPath_);
			/** @brief Assets without resources (ApkAssets.loadEmpty) */
			static std::shared_ptr<ApkAssets> loadEmpty(const std::string& assetPath_);

			/** @brief Path as seen by Java (Android path) */
			const std::string& getAssetPath() const {
				return _assetPath;
			}
			/** @brief Resource table, nullptr if the container has no resources.arsc */
			const ResourceTable* getTable() const {
				return _table.get();
			}
			/** @brief Global string pool of the table, nullptr if no table */
			std::shared_ptr<StringPool> getStringPool() const {
				return _table ? _table->getGlobalStrings() : nullptr;
			}
			/** @brief Reads a file of the APK ("res/layout/main.xml", "assets/..."), nullopt if missing */
			std::optional<std::vector<uint8_t>> readFile(const std::string& name_) const;

		private:
			explicit ApkAssets(std::string assetPath_);

			std::string _assetPath;
			std::unique_ptr<ZipReader> _zip;
			mutable std::mutex _zipMutex;
			std::shared_ptr<std::vector<uint8_t>> _tableData;
			std::unique_ptr<ResourceTable> _table;
	};
}  // namespace sandvik::resources

#endif  // __RESOURCES_APK_ASSETS_HPP__
