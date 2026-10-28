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

#include "resources/resource_table.hpp"

#include <algorithm>

#include "system/logger.hpp"
#include "utils.hpp"

using namespace sandvik;
using namespace sandvik::resources;

std::optional<ByteView> TypeVariant::entryAt(uint16_t entryIndex_) const {
	size_t offset = 0;
	if (sparse) {
		// entries sorted by index: binary search
		size_t low = 0;
		size_t high = entryCount;
		bool found = false;
		while (low < high) {
			const size_t mid = (low + high) / 2;
			const auto entry = chunk.read<ResTableSparseTypeEntry>(headerSize + mid * sizeof(ResTableSparseTypeEntry));
			if (entry.idx == entryIndex_) {
				offset = static_cast<size_t>(entry.offset) * 4;
				found = true;
				break;
			}
			if (entry.idx < entryIndex_) {
				low = mid + 1;
			} else {
				high = mid;
			}
		}
		if (!found) {
			return std::nullopt;
		}
	} else {
		if (entryIndex_ >= entryCount) {
			return std::nullopt;
		}
		const auto entryOffset = chunk.read<uint32_t>(headerSize + static_cast<size_t>(entryIndex_) * sizeof(uint32_t));
		if (entryOffset == NO_ENTRY) {
			return std::nullopt;
		}
		offset = entryOffset;
	}
	const size_t start = static_cast<size_t>(entriesStart) + offset;
	if (start >= chunk.size()) {
		throw VmException("resources: entry {} out of its type chunk", entryIndex_);
	}
	return chunk.sub(start, chunk.size() - start);
}

ResValue ResolvedEntry::value() const {
	return entry.read<ResValue>(header.size);
}

uint32_t ResolvedEntry::parent() const {
	return entry.read<ResTableMapEntry>(0).parent;
}

std::vector<ResTableMap> ResolvedEntry::mapItems() const {
	const auto mapEntry = entry.read<ResTableMapEntry>(0);
	std::vector<ResTableMap> items;
	items.reserve(mapEntry.count);
	for (uint32_t i = 0; i < mapEntry.count; ++i) {
		items.push_back(entry.read<ResTableMap>(header.size + static_cast<size_t>(i) * sizeof(ResTableMap)));
	}
	return items;
}

ResourcePackage::ResourcePackage(ByteView chunk_, std::shared_ptr<const void> owner_) {
	const auto chunkHeader = chunk_.read<ResChunkHeader>(0);
	const auto header = chunk_.readPartial<ResTablePackage>(0, chunkHeader.headerSize);
	_id = static_cast<uint8_t>(header.id);
	_typeIdOffset = header.typeIdOffset;
	std::u16string name;
	for (char16_t c : header.name) {
		if (c == 0) {
			break;
		}
		name.push_back(c);
	}
	_name = utf16_to_utf8(name);

	auto poolAt = [&chunk_, &owner_](uint32_t offset_) {
		const auto poolHeader = chunk_.read<ResChunkHeader>(offset_);
		return StringPool(chunk_.sub(offset_, poolHeader.size), owner_);
	};
	_typeStrings = poolAt(header.typeStrings);
	_keyStrings = poolAt(header.keyStrings);

	for (size_t pos = chunkHeader.headerSize; pos + sizeof(ResChunkHeader) <= chunk_.size();) {
		const auto child = chunk_.read<ResChunkHeader>(pos);
		if (child.size < sizeof(ResChunkHeader)) {
			throw VmException("resources: invalid chunk size {} in package {}", child.size, _name);
		}
		const auto childView = chunk_.sub(pos, child.size);
		switch (child.type) {
			case RES_TABLE_TYPE_SPEC_TYPE:
				parseTypeSpec(childView);
				break;
			case RES_TABLE_TYPE_TYPE:
				parseType(childView);
				break;
			case RES_STRING_POOL_TYPE:
				// type and key strings, already loaded
				break;
			default:
				// library (dynamic references), overlayable, staged aliases: not used by Sandvik yet
				logger.fdebug("resources: package {} chunk {:#x} ignored", _name, child.type);
				break;
		}
		pos += child.size;
	}
}

ResourceType& ResourcePackage::typeFor(uint8_t typeId_) {
	if (typeId_ == 0) {
		throw VmException("resources: invalid type id 0 in package {}", _name);
	}
	if (_types.size() < typeId_) {
		_types.resize(typeId_);
	}
	auto& type = _types[typeId_ - 1];
	if (!type) {
		type = std::make_unique<ResourceType>();
		type->id = typeId_;
	}
	return *type;
}

void ResourcePackage::parseTypeSpec(ByteView chunk_) {
	const auto spec = chunk_.read<ResTableTypeSpec>(0);
	auto& type = typeFor(spec.id);
	type.specFlags.resize(spec.entryCount);
	for (uint32_t i = 0; i < spec.entryCount; ++i) {
		type.specFlags[i] = chunk_.read<uint32_t>(spec.header.headerSize + static_cast<size_t>(i) * sizeof(uint32_t));
	}
}

void ResourcePackage::parseType(ByteView chunk_) {
	// ResTable_type: header, id, flags, reserved, entryCount, entriesStart, then the configuration up to headerSize
	const auto header = chunk_.read<ResChunkHeader>(0);
	TypeVariant variant;
	const auto typeId = chunk_.read<uint8_t>(8);
	variant.sparse = (chunk_.read<uint8_t>(9) & TYPE_FLAG_SPARSE) != 0;
	variant.entryCount = chunk_.read<uint32_t>(12);
	variant.entriesStart = chunk_.read<uint32_t>(16);
	variant.config = ResConfig::read(chunk_, 20);
	variant.chunk = chunk_;
	variant.headerSize = header.headerSize;
	typeFor(typeId).variants.push_back(variant);
}

const ResourceType* ResourcePackage::getType(uint8_t typeId_) const {
	if (typeId_ == 0 || typeId_ > _types.size()) {
		return nullptr;
	}
	return _types[typeId_ - 1].get();
}

std::string ResourcePackage::getTypeName(uint8_t typeId_) const {
	if (typeId_ == 0 || typeId_ <= _typeIdOffset) {
		return "";
	}
	return _typeStrings.string8At(typeId_ - 1 - _typeIdOffset).value_or("");
}

std::optional<ResolvedEntry> ResourcePackage::findEntry(uint8_t typeId_, uint16_t entryId_, const ResConfig& requested_) const {
	const auto* type = getType(typeId_);
	if (type == nullptr) {
		return std::nullopt;
	}
	const TypeVariant* best = nullptr;
	ByteView bestEntry;
	for (const auto& variant : type->variants) {
		if (!variant.config.match(requested_)) {
			continue;
		}
		if (best != nullptr && !variant.config.isBetterThan(best->config, requested_)) {
			continue;
		}
		if (auto entry = variant.entryAt(entryId_)) {
			best = &variant;
			bestEntry = *entry;
		}
	}
	if (best == nullptr) {
		return std::nullopt;
	}
	ResolvedEntry result;
	result.entry = bestEntry;
	result.config = best->config;
	result.specFlags = entryId_ < type->specFlags.size() ? type->specFlags[entryId_] : 0;
	result.header = bestEntry.read<ResTableEntry>(0);
	return result;
}

std::string ResourcePackage::getEntryName(uint8_t typeId_, uint16_t entryId_) const {
	const auto* type = getType(typeId_);
	if (type == nullptr) {
		return "";
	}
	for (const auto& variant : type->variants) {
		if (auto entry = variant.entryAt(entryId_)) {
			return _keyStrings.string8At(entry->read<ResTableEntry>(0).key).value_or("");
		}
	}
	return "";
}

uint32_t ResourcePackage::findResourceId(const std::string& typeName_, const std::string& entryName_) const {
	std::call_once(_namesBuilt, [this]() {
		for (const auto& type : _types) {
			if (!type) {
				continue;
			}
			const auto typeName = getTypeName(type->id);
			for (const auto& variant : type->variants) {
				variant.forEachEntry([this, &type, &typeName](uint16_t entryId_, const ByteView& entry_) {
					auto name = _keyStrings.string8At(entry_.read<ResTableEntry>(0).key).value_or("");
					_names.emplace(typeName + "/" + name, makeResId(_id, type->id, entryId_));
				});
			}
		}
	});
	auto it = _names.find(typeName_ + "/" + entryName_);
	return it == _names.end() ? 0 : it->second;
}

void ResourcePackage::collectLocales(std::vector<std::string>& locales_) const {
	for (const auto& type : _types) {
		if (!type) {
			continue;
		}
		for (const auto& variant : type->variants) {
			auto locale = variant.config.getBcp47Locale();
			if (!locale.empty() && std::find(locales_.begin(), locales_.end(), locale) == locales_.end()) {
				locales_.push_back(locale);
			}
		}
	}
}

ResourceTable::ResourceTable(ByteView data_, std::shared_ptr<const void> owner_) {
	const auto header = data_.read<ResTableHeader>(0);
	if (header.header.type != RES_TABLE_TYPE) {
		throw VmException("resources: not a resource table (chunk type {:#x})", header.header.type);
	}
	const auto table = data_.sub(0, std::min<size_t>(header.header.size, data_.size()));
	for (size_t pos = header.header.headerSize; pos + sizeof(ResChunkHeader) <= table.size();) {
		const auto child = table.read<ResChunkHeader>(pos);
		if (child.size < sizeof(ResChunkHeader)) {
			throw VmException("resources: invalid chunk size {} in resource table", child.size);
		}
		const auto childView = table.sub(pos, child.size);
		if (child.type == RES_STRING_POOL_TYPE) {
			_globalStrings = std::make_shared<StringPool>(childView, owner_);
		} else if (child.type == RES_TABLE_PACKAGE_TYPE) {
			_packages.push_back(std::make_unique<ResourcePackage>(childView, owner_));
		}
		pos += child.size;
	}
	if (!_globalStrings) {
		throw VmException("resources: resource table without string pool");
	}
}

const ResourcePackage* ResourceTable::findPackage(uint8_t packageId_) const {
	for (const auto& package : _packages) {
		if (package->getId() == packageId_) {
			return package.get();
		}
	}
	return nullptr;
}
