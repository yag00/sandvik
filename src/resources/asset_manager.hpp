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

#ifndef __RESOURCES_ASSET_MANAGER_HPP__
#define __RESOURCES_ASSET_MANAGER_HPP__

#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "resources/apk_assets.hpp"

namespace sandvik::resources {
	/** @brief Index of an ApkAssets in its AssetManager, kInvalidCookie if none (androidfw ApkAssetsCookie) */
	using Cookie = int32_t;
	constexpr Cookie INVALID_COOKIE = -1;

	/** @brief A resolved value and where it comes from (androidfw AssetManager2::SelectedValue) */
	struct SelectedValue {
			ResValue value;
			Cookie cookie = INVALID_COOKIE;
			uint32_t flags = 0;    // configuration changes the value depends on
			uint32_t resid = 0;    // last resource id followed to get the value
			uint16_t density = 0;  // density of the selected configuration
	};

	/** @brief A complex resource (style, array...) merged with its parents, items sorted by key */
	struct Bag {
			struct Item {
					uint32_t key;
					ResValue value;
					Cookie cookie;
			};
			std::vector<Item> items;
			uint32_t typeSpecFlags = 0;

			const Item* find(uint32_t key_) const;
	};

	/** @brief Package, type and entry names of a resource */
	struct ResourceName {
			std::string package;
			std::string type;
			std::string entry;
	};

	/**
	 * @class AssetManager
	 * @brief Resource lookups over an ordered set of ApkAssets for a configuration (androidfw AssetManager2).
	 *
	 * The cookie of a value is the index of the ApkAssets that defines it; Java uses cookie + 1.
	 */
	class AssetManager {
		public:
			void setApkAssets(std::vector<std::shared_ptr<ApkAssets>> apkAssets_);
			const std::vector<std::shared_ptr<ApkAssets>>& getApkAssets() const {
				return _apkAssets;
			}
			void setConfiguration(const ResConfig& config_);
			const ResConfig& getConfiguration() const {
				return _config;
			}

			/** @brief Value of a resource for the current configuration (no reference resolution).
			 * @param mayBeBag_ if false a complex resource is an error, if true it is returned as a reference to itself
			 * @param densityOverride_ density to use instead of the configuration one, 0 for none */
			std::optional<SelectedValue> getResource(uint32_t resid_, bool mayBeBag_, uint16_t densityOverride_ = 0) const;
			/** @brief Follows TYPE_REFERENCE values until a final value (or a bag reference) */
			bool resolveReference(SelectedValue& value_) const;
			/** @brief Bag of a complex resource merged with its parents, nullptr if not a bag */
			std::shared_ptr<const Bag> getBag(uint32_t resid_) const;

			std::optional<ResourceName> getResourceName(uint32_t resid_) const;
			/** @brief Resource id from "package:type/entry" (or parts given as defaults), 0 if not found */
			uint32_t getResourceId(const std::string& name_, const std::string& defType_, const std::string& defPackage_) const;
			/** @brief Package ids and names of the loaded tables */
			std::vector<std::pair<uint8_t, std::string>> getAssignedPackages() const;
			std::vector<std::string> getLocales() const;
			/** @brief String pool of the ApkAssets of a cookie, nullptr if none */
			std::shared_ptr<StringPool> getStringPool(Cookie cookie_) const;

		private:
			struct PackageRef {
					Cookie cookie;
					const ResourcePackage* package;
			};
			std::optional<std::pair<PackageRef, ResolvedEntry>> findEntry(uint32_t resid_, const ResConfig& config_) const;
			/** @brief Dynamic references point to the defining package when their package id is 0 */
			static ResValue normalize(ResValue value_, uint8_t packageId_);

			std::vector<std::shared_ptr<ApkAssets>> _apkAssets;
			std::map<uint8_t, std::vector<PackageRef>> _packages;  // package id -> packages, last loaded wins
			ResConfig _config;
			mutable std::mutex _bagMutex;
			mutable std::unordered_map<uint32_t, std::shared_ptr<const Bag>> _bags;  // cleared on configuration change
	};

	/**
	 * @class Theme
	 * @brief Attribute values of applied styles (androidfw Theme).
	 */
	class Theme {
		public:
			explicit Theme(const AssetManager& assetManager_) : _assetManager(&assetManager_) {
			}
			/** @brief Applies the attributes of a style; existing attributes are kept unless force_ */
			bool applyStyle(uint32_t style_, bool force_);
			/** @brief Value of an attribute, following attribute references (?attr) to the final value */
			std::optional<SelectedValue> getAttribute(uint32_t attr_) const;
			/** @brief Replaces an attribute reference by its value in the theme, then resolves references */
			bool resolveAttributeReference(SelectedValue& value_) const;
			void clear();
			/** @brief Copy of the attributes of another theme */
			void setTo(const Theme& other_);
			uint32_t getChangingConfigurations() const {
				return _typeSpecFlags;
			}
			const AssetManager& getAssetManager() const {
				return *_assetManager;
			}

		private:
			struct Entry {
					ResValue value;
					Cookie cookie;
					uint32_t typeSpecFlags;
			};
			const AssetManager* _assetManager;
			std::unordered_map<uint32_t, Entry> _entries;
			uint32_t _typeSpecFlags = 0;
	};
}  // namespace sandvik::resources

#endif  // __RESOURCES_ASSET_MANAGER_HPP__
