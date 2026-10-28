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

#include "resources/asset_manager.hpp"

#include <algorithm>

#include "system/logger.hpp"

using namespace sandvik::resources;

namespace {
	// Same limit as androidfw: reference and parent chains longer than this are considered circular
	constexpr int MAX_ITERATIONS = 20;
}  // namespace

const Bag::Item* Bag::find(uint32_t key_) const {
	auto it = std::lower_bound(items.begin(), items.end(), key_, [](const Item& item_, uint32_t key) { return item_.key < key; });
	return (it != items.end() && it->key == key_) ? &*it : nullptr;
}

void AssetManager::setApkAssets(std::vector<std::shared_ptr<ApkAssets>> apkAssets_) {
	_apkAssets = std::move(apkAssets_);
	_packages.clear();
	for (size_t cookie = 0; cookie < _apkAssets.size(); ++cookie) {
		const auto* table = _apkAssets[cookie]->getTable();
		if (table == nullptr) {
			continue;
		}
		for (const auto& package : table->getPackages()) {
			_packages[package->getId()].push_back(PackageRef{static_cast<Cookie>(cookie), package.get()});
		}
	}
	std::lock_guard lock(_bagMutex);
	_bags.clear();
}

void AssetManager::setConfiguration(const ResConfig& config_) {
	_config = config_;
	std::lock_guard lock(_bagMutex);
	_bags.clear();
}

ResValue AssetManager::normalize(ResValue value_, uint8_t packageId_) {
	if (value_.dataType == TYPE_DYNAMIC_REFERENCE || value_.dataType == TYPE_DYNAMIC_ATTRIBUTE) {
		value_.dataType = value_.dataType == TYPE_DYNAMIC_REFERENCE ? TYPE_REFERENCE : TYPE_ATTRIBUTE;
		if (value_.data != 0 && getPackageId(value_.data) == 0) {
			value_.data |= static_cast<uint32_t>(packageId_) << 24;
		}
	}
	return value_;
}

std::optional<std::pair<AssetManager::PackageRef, ResolvedEntry>> AssetManager::findEntry(uint32_t resid_, const ResConfig& config_) const {
	if (!isValidResId(resid_)) {
		return std::nullopt;
	}
	auto it = _packages.find(getPackageId(resid_));
	if (it == _packages.end()) {
		return std::nullopt;
	}
	std::optional<std::pair<PackageRef, ResolvedEntry>> best;
	for (const auto& ref : it->second) {
		auto entry = ref.package->findEntry(getTypeId(resid_), getEntryId(resid_), config_);
		if (entry && (!best || !best->second.config.isBetterThan(entry->config, config_))) {
			best = std::make_pair(ref, *entry);
		}
	}
	return best;
}

std::optional<SelectedValue> AssetManager::getResource(uint32_t resid_, bool mayBeBag_, uint16_t densityOverride_) const {
	ResConfig config = _config;
	if (densityOverride_ != 0) {
		config.density = densityOverride_;
	}
	auto found = findEntry(resid_, config);
	if (!found) {
		return std::nullopt;
	}
	const auto& [ref, entry] = *found;
	SelectedValue result;
	result.cookie = ref.cookie;
	result.flags = entry.specFlags;
	result.resid = resid_;
	result.density = entry.config.density;
	if (entry.isComplex()) {
		if (!mayBeBag_) {
			logger.fdebug("resources: resource {:#010x} is a complex map type", resid_);
			return std::nullopt;
		}
		// a bag can't be a Res_value: it is returned as a reference to itself
		result.value.dataType = TYPE_REFERENCE;
		result.value.data = resid_;
		return result;
	}
	result.value = normalize(entry.value(), ref.package->getId());
	return result;
}

bool AssetManager::resolveReference(SelectedValue& value_) const {
	for (int i = 0; i < MAX_ITERATIONS; ++i) {
		if (value_.value.dataType != TYPE_REFERENCE || value_.value.data == 0) {
			return true;
		}
		const uint32_t resid = value_.value.data;
		auto result = getResource(resid, true);
		if (!result) {
			return false;
		}
		const uint32_t flags = value_.flags | result->flags;
		if (result->value.dataType == TYPE_REFERENCE && result->value.data == resid) {
			// reference to a bag: it is the final value
			value_.flags = flags;
			value_.resid = resid;
			value_.cookie = result->cookie;
			return true;
		}
		value_ = *result;
		value_.flags = flags;
	}
	logger.fwarning("resources: too many references resolving {:#010x}", value_.value.data);
	return false;
}

std::shared_ptr<const Bag> AssetManager::getBag(uint32_t resid_) const {
	{
		std::lock_guard lock(_bagMutex);
		if (auto it = _bags.find(resid_); it != _bags.end()) {
			return it->second;
		}
	}
	// a style inherits the items of its parent chain, the child items override the parent ones
	std::vector<uint32_t> chain;
	auto bag = std::make_shared<Bag>();
	uint32_t current = resid_;
	std::vector<std::pair<PackageRef, ResolvedEntry>> entries;
	for (int depth = 0; current != 0; ++depth) {
		if (depth >= MAX_ITERATIONS || std::find(chain.begin(), chain.end(), current) != chain.end()) {
			logger.fwarning("resources: circular or too deep parent chain for bag {:#010x}", resid_);
			break;
		}
		chain.push_back(current);
		auto found = findEntry(current, _config);
		if (!found || !found->second.isComplex()) {
			if (depth == 0) {
				return nullptr;
			}
			logger.fdebug("resources: bag {:#010x} has an invalid parent {:#010x}", resid_, current);
			break;
		}
		entries.push_back(*found);
		current = found->second.parent();
	}
	// apply from the root parent to the bag itself
	for (auto it = entries.rbegin(); it != entries.rend(); ++it) {
		const auto& [ref, entry] = *it;
		bag->typeSpecFlags |= entry.specFlags;
		for (const auto& item : entry.mapItems()) {
			// attribute of a shared library: package id 0 means the defining package
			uint32_t key = item.name;
			if (getPackageId(key) == 0 && getTypeId(key) != 0) {
				key |= static_cast<uint32_t>(ref.package->getId()) << 24;
			}
			Bag::Item bagItem{key, normalize(item.value, ref.package->getId()), ref.cookie};
			auto pos = std::lower_bound(bag->items.begin(), bag->items.end(), key, [](const Bag::Item& item_, uint32_t k) { return item_.key < k; });
			if (pos != bag->items.end() && pos->key == key) {
				*pos = bagItem;
			} else {
				bag->items.insert(pos, bagItem);
			}
		}
	}
	std::lock_guard lock(_bagMutex);
	_bags[resid_] = bag;
	return bag;
}

std::optional<ResourceName> AssetManager::getResourceName(uint32_t resid_) const {
	auto it = _packages.find(getPackageId(resid_));
	if (it == _packages.end()) {
		return std::nullopt;
	}
	for (const auto& ref : it->second) {
		auto entry = ref.package->getEntryName(getTypeId(resid_), getEntryId(resid_));
		if (!entry.empty()) {
			return ResourceName{ref.package->getName(), ref.package->getTypeName(getTypeId(resid_)), entry};
		}
	}
	return std::nullopt;
}

uint32_t AssetManager::getResourceId(const std::string& name_, const std::string& defType_, const std::string& defPackage_) const {
	// [@][package:]type/entry, with defaults for the missing parts
	std::string name = name_;
	if (!name.empty() && (name[0] == '@' || name[0] == '?')) {
		name = name.substr(1);
	}
	std::string package = defPackage_;
	std::string type = defType_;
	if (auto colon = name.find(':'); colon != std::string::npos) {
		package = name.substr(0, colon);
		name = name.substr(colon + 1);
	}
	if (auto slash = name.find('/'); slash != std::string::npos) {
		type = name.substr(0, slash);
		name = name.substr(slash + 1);
	}
	if (type.empty() || name.empty()) {
		return 0;
	}
	for (const auto& [id, refs] : _packages) {
		for (const auto& ref : refs) {
			if (!package.empty() && ref.package->getName() != package) {
				continue;
			}
			if (auto resid = ref.package->findResourceId(type, name); resid != 0) {
				return resid;
			}
		}
	}
	return 0;
}

std::vector<std::pair<uint8_t, std::string>> AssetManager::getAssignedPackages() const {
	std::vector<std::pair<uint8_t, std::string>> result;
	for (const auto& [id, refs] : _packages) {
		if (!refs.empty()) {
			result.emplace_back(id, refs.front().package->getName());
		}
	}
	return result;
}

std::vector<std::string> AssetManager::getLocales() const {
	std::vector<std::string> locales;
	for (const auto& [id, refs] : _packages) {
		for (const auto& ref : refs) {
			ref.package->collectLocales(locales);
		}
	}
	return locales;
}

std::shared_ptr<StringPool> AssetManager::getStringPool(Cookie cookie_) const {
	if (cookie_ < 0 || static_cast<size_t>(cookie_) >= _apkAssets.size()) {
		return nullptr;
	}
	return _apkAssets[cookie_]->getStringPool();
}

bool Theme::applyStyle(uint32_t style_, bool force_) {
	auto bag = _assetManager->getBag(style_);
	if (!bag) {
		return false;
	}
	for (const auto& item : bag->items) {
		// only attributes can be set in a theme
		if (!isValidResId(item.key)) {
			continue;
		}
		auto it = _entries.find(item.key);
		const bool undefined = it == _entries.end() || (it->second.value.dataType == TYPE_NULL && it->second.value.data != DATA_NULL_EMPTY);
		if (force_ || undefined) {
			_entries[item.key] = Entry{item.value, item.cookie, bag->typeSpecFlags};
		}
	}
	_typeSpecFlags |= bag->typeSpecFlags;
	return true;
}

std::optional<SelectedValue> Theme::getAttribute(uint32_t attr_) const {
	uint32_t flags = 0;
	uint32_t attr = attr_;
	for (int i = 0; i < MAX_ITERATIONS; ++i) {
		auto it = _entries.find(attr);
		if (it == _entries.end()) {
			return std::nullopt;
		}
		const auto& entry = it->second;
		flags |= entry.typeSpecFlags;
		if (entry.value.dataType == TYPE_ATTRIBUTE) {
			// ?attr: the value is the one of another attribute of the theme
			attr = entry.value.data;
			continue;
		}
		if (entry.value.dataType == TYPE_NULL && entry.value.data != DATA_NULL_EMPTY) {
			return std::nullopt;
		}
		SelectedValue result;
		result.value = entry.value;
		result.cookie = entry.cookie;
		result.flags = flags;
		return result;
	}
	logger.fwarning("resources: too many attribute references resolving {:#010x}", attr_);
	return std::nullopt;
}

bool Theme::resolveAttributeReference(SelectedValue& value_) const {
	if (value_.value.dataType != TYPE_ATTRIBUTE) {
		return _assetManager->resolveReference(value_);
	}
	auto result = getAttribute(value_.value.data);
	if (!result) {
		return false;
	}
	if (!_assetManager->resolveReference(*result)) {
		return false;
	}
	result->flags |= value_.flags;
	value_ = *result;
	return true;
}

void Theme::clear() {
	_entries.clear();
	_typeSpecFlags = 0;
}

void Theme::setTo(const Theme& other_) {
	_entries = other_._entries;
	_typeSpecFlags = other_._typeSpecFlags;
}
