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

#ifndef __RESOURCES_RESOURCE_TABLE_HPP__
#define __RESOURCES_RESOURCE_TABLE_HPP__

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "resources/config.hpp"
#include "resources/string_pool.hpp"

namespace sandvik::resources {
	/** @brief One configuration variant of a type (one ResTable_type chunk: values, values-fr, values-v21...) */
	struct TypeVariant {
			ResConfig config;
			ByteView chunk;  // whole ResTable_type chunk
			uint32_t entryCount = 0;
			uint32_t entriesStart = 0;
			bool sparse = false;
			uint16_t headerSize = 0;

			/** @brief View starting at the ResTable_entry of an entry, nullopt if this variant does not define it */
			std::optional<ByteView> entryAt(uint16_t entryIndex_) const;
			/** @brief Calls visitor_(entryIndex, view) for each entry defined by this variant */
			template <typename Visitor>
			void forEachEntry(Visitor visitor_) const {
				for (uint32_t i = 0; i < entryCount; ++i) {
					const uint16_t index = sparse
					                           ? chunk.read<ResTableSparseTypeEntry>(headerSize + static_cast<size_t>(i) * sizeof(ResTableSparseTypeEntry)).idx
					                           : static_cast<uint16_t>(i);
					if (auto entry = entryAt(index)) {
						visitor_(index, *entry);
					}
				}
			}
	};

	/** @brief A resource type of a package (attr, string, style...) and all its variants */
	struct ResourceType {
			uint8_t id = 0;
			std::vector<uint32_t> specFlags;  // per entry: configuration changes the value depends on
			std::vector<TypeVariant> variants;
	};

	/** @brief An entry selected for a configuration */
	struct ResolvedEntry {
			ByteView entry;  // starts at the ResTable_entry
			ResConfig config;
			uint32_t specFlags = 0;
			ResTableEntry header{};

			bool isComplex() const {
				return (header.flags & ENTRY_FLAG_COMPLEX) != 0;
			}
			/** @brief Value of a simple entry */
			ResValue value() const;
			/** @brief Parent style/bag of a complex entry, 0 if none */
			uint32_t parent() const;
			/** @brief Items of a complex entry */
			std::vector<ResTableMap> mapItems() const;
	};

	/**
	 * @class ResourcePackage
	 * @brief A package of a resource table (androidfw LoadedPackage): "android" (0x01), the application (0x7f)...
	 */
	class ResourcePackage {
		public:
			ResourcePackage(ByteView chunk_, std::shared_ptr<const void> owner_);

			uint8_t getId() const {
				return _id;
			}
			const std::string& getName() const {
				return _name;
			}
			const StringPool& getTypeStrings() const {
				return _typeStrings;
			}
			const StringPool& getKeyStrings() const {
				return _keyStrings;
			}
			/** @brief Type by id (1 based), nullptr if not defined */
			const ResourceType* getType(uint8_t typeId_) const;
			/** @brief Name of a type ("string", "style"...) */
			std::string getTypeName(uint8_t typeId_) const;
			/** @brief Best entry for the requested configuration, nullopt if the resource is not defined for it */
			std::optional<ResolvedEntry> findEntry(uint8_t typeId_, uint16_t entryId_, const ResConfig& requested_) const;
			/** @brief Name of the entry (key string), empty if unknown */
			std::string getEntryName(uint8_t typeId_, uint16_t entryId_) const;
			/** @brief Resource id from a type and entry name, 0 if not found */
			uint32_t findResourceId(const std::string& typeName_, const std::string& entryName_) const;
			/** @brief Locales of all the variants */
			void collectLocales(std::vector<std::string>& locales_) const;

		private:
			void parseTypeSpec(ByteView chunk_);
			void parseType(ByteView chunk_);
			ResourceType& typeFor(uint8_t typeId_);

			uint8_t _id = 0;
			std::string _name;
			uint32_t _typeIdOffset = 0;
			StringPool _typeStrings;
			StringPool _keyStrings;
			std::vector<std::unique_ptr<ResourceType>> _types;  // index typeId - 1
			// name lookups, built on first use: "type/entry" -> resid
			mutable std::once_flag _namesBuilt;
			mutable std::unordered_map<std::string, uint32_t> _names;
	};

	/**
	 * @class ResourceTable
	 * @brief A compiled resource table, resources.arsc (androidfw LoadedArsc).
	 */
	class ResourceTable {
		public:
			/** @brief Parses a resources.arsc
			 * @param data_ table bytes
			 * @param owner_ buffer the view points into, kept alive by the table */
			ResourceTable(ByteView data_, std::shared_ptr<const void> owner_);

			/** @brief Pool of the string values of the table (TYPE_STRING data indexes it) */
			const std::shared_ptr<StringPool>& getGlobalStrings() const {
				return _globalStrings;
			}
			const std::vector<std::unique_ptr<ResourcePackage>>& getPackages() const {
				return _packages;
			}
			const ResourcePackage* findPackage(uint8_t packageId_) const;

		private:
			std::shared_ptr<StringPool> _globalStrings;
			std::vector<std::unique_ptr<ResourcePackage>> _packages;
	};
}  // namespace sandvik::resources

#endif  // __RESOURCES_RESOURCE_TABLE_HPP__
