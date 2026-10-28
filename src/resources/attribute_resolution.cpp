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

#include "resources/attribute_resolution.hpp"

using namespace sandvik::resources;

namespace {
	bool isUndefined(const SelectedValue& value_) {
		return value_.value.dataType == TYPE_NULL && value_.value.data != DATA_NULL_EMPTY;
	}

	SelectedValue fromBagItem(const Bag& bag_, const Bag::Item& item_) {
		SelectedValue value;
		value.value = item_.value;
		value.cookie = item_.cookie;
		value.flags = bag_.typeSpecFlags;
		return value;
	}

	/** @brief Writes one attribute in the TypedArray layout, returns true if the attribute is defined */
	bool writeValue(SelectedValue value_, uint32_t sourceResId_, int32_t* out_) {
		if (value_.value.dataType == TYPE_REFERENCE && value_.value.data == 0) {
			// @null is an undefined value
			value_.value.dataType = TYPE_NULL;
			value_.value.data = DATA_NULL_UNDEFINED;
			value_.cookie = INVALID_COOKIE;
		}
		out_[STYLE_TYPE] = value_.value.dataType;
		out_[STYLE_DATA] = static_cast<int32_t>(value_.value.data);
		// Java cookies are 1 based, -1 when the value does not come from an ApkAssets (XML string)
		out_[STYLE_ASSET_COOKIE] = value_.cookie == INVALID_COOKIE ? -1 : value_.cookie + 1;
		out_[STYLE_RESOURCE_ID] = static_cast<int32_t>(value_.resid);
		out_[STYLE_CHANGING_CONFIGURATIONS] = static_cast<int32_t>(value_.flags);
		out_[STYLE_DENSITY] = value_.density;
		out_[STYLE_SOURCE_RESOURCE_ID] = static_cast<int32_t>(sourceResId_);
		return value_.value.dataType != TYPE_NULL || value_.value.data == DATA_NULL_EMPTY;
	}

	/** @brief defStyleAttr, when the theme defines it, overrides defStyleRes */
	uint32_t resolveDefaultStyle(const Theme& theme_, uint32_t defStyleAttr_, uint32_t defStyleRes_) {
		if (defStyleAttr_ != 0) {
			if (auto value = theme_.getAttribute(defStyleAttr_); value && value->value.dataType == TYPE_REFERENCE) {
				return value->value.data;
			}
		}
		return defStyleRes_;
	}

	void prepare(size_t count_, std::vector<int32_t>& outValues_, std::vector<int32_t>& outIndices_) {
		outValues_.assign(count_ * STYLE_NUM_ENTRIES, 0);
		outIndices_.assign(count_ + 1, 0);
	}

	/** @brief Value of attr_ set in XML, undefined if the XML does not set it */
	SelectedValue xmlValue(const XmlAttributes* xml_, uint32_t attr_) {
		SelectedValue value;
		if (xml_ == nullptr) {
			return value;
		}
		for (size_t i = 0; i < xml_->getAttributeCount(); ++i) {
			if (xml_->getAttributeNameResId(i) == attr_) {
				value.value = xml_->getAttributeValue(i);
				break;
			}
		}
		return value;
	}
}  // namespace

void sandvik::resources::applyStyle(const Theme& theme_, const XmlAttributes* xml_, uint32_t defStyleAttr_, uint32_t defStyleRes_,
                                    const std::vector<uint32_t>& attrs_, std::vector<int32_t>& outValues_, std::vector<int32_t>& outIndices_) {
	const auto& assetManager = theme_.getAssetManager();
	prepare(attrs_.size(), outValues_, outIndices_);
	const uint32_t defStyle = resolveDefaultStyle(theme_, defStyleAttr_, defStyleRes_);
	const auto defStyleBag = defStyle != 0 ? assetManager.getBag(defStyle) : nullptr;
	const uint32_t xmlStyle = xml_ != nullptr ? xml_->getStyleResId() : 0;
	const auto xmlStyleBag = xmlStyle != 0 ? assetManager.getBag(xmlStyle) : nullptr;

	int32_t defined = 0;
	for (size_t i = 0; i < attrs_.size(); ++i) {
		const uint32_t attr = attrs_[i];
		// priority: XML attribute, style="" of the element, default style, theme
		SelectedValue value = xmlValue(xml_, attr);
		uint32_t source = isUndefined(value) ? 0 : xml_->getSourceResId();
		if (isUndefined(value) && xmlStyleBag) {
			if (const auto* item = xmlStyleBag->find(attr)) {
				value = fromBagItem(*xmlStyleBag, *item);
				source = xmlStyle;
			}
		}
		if (isUndefined(value) && defStyleBag) {
			if (const auto* item = defStyleBag->find(attr)) {
				value = fromBagItem(*defStyleBag, *item);
				source = defStyle;
			}
		}
		if (value.value.dataType != TYPE_NULL) {
			// a value that can't be resolved is kept as is, like androidfw
			theme_.resolveAttributeReference(value);
		} else if (value.value.data != DATA_NULL_EMPTY) {
			if (auto themeValue = theme_.getAttribute(attr)) {
				value = *themeValue;
				assetManager.resolveReference(value);
				source = 0;
			}
		}
		if (writeValue(value, source, &outValues_[i * STYLE_NUM_ENTRIES])) {
			outIndices_[++defined] = static_cast<int32_t>(i);
		}
	}
	outIndices_[0] = defined;
}

bool sandvik::resources::resolveAttrs(const Theme& theme_, uint32_t defStyleAttr_, uint32_t defStyleRes_, const std::vector<uint32_t>& srcValues_,
                                      const std::vector<uint32_t>& attrs_, std::vector<int32_t>& outValues_, std::vector<int32_t>& outIndices_) {
	const auto& assetManager = theme_.getAssetManager();
	prepare(attrs_.size(), outValues_, outIndices_);
	const uint32_t defStyle = resolveDefaultStyle(theme_, defStyleAttr_, defStyleRes_);
	const auto defStyleBag = defStyle != 0 ? assetManager.getBag(defStyle) : nullptr;

	int32_t defined = 0;
	for (size_t i = 0; i < attrs_.size(); ++i) {
		const uint32_t attr = attrs_[i];
		SelectedValue value;
		uint32_t source = 0;
		// a given source value is an attribute reference (?attr)
		if (i < srcValues_.size() && srcValues_[i] != 0) {
			value.value.dataType = TYPE_ATTRIBUTE;
			value.value.data = srcValues_[i];
		}
		if (isUndefined(value) && defStyleBag) {
			if (const auto* item = defStyleBag->find(attr)) {
				value = fromBagItem(*defStyleBag, *item);
				source = defStyle;
			}
		}
		if (value.value.dataType != TYPE_NULL) {
			theme_.resolveAttributeReference(value);
		} else if (value.value.data != DATA_NULL_EMPTY) {
			if (auto themeValue = theme_.getAttribute(attr)) {
				value = *themeValue;
				assetManager.resolveReference(value);
			}
		}
		if (writeValue(value, source, &outValues_[i * STYLE_NUM_ENTRIES])) {
			outIndices_[++defined] = static_cast<int32_t>(i);
		}
	}
	outIndices_[0] = defined;
	return true;
}

bool sandvik::resources::retrieveAttributes(const AssetManager& assetManager_, const XmlAttributes* xml_, const std::vector<uint32_t>& attrs_,
                                            std::vector<int32_t>& outValues_, std::vector<int32_t>& outIndices_) {
	prepare(attrs_.size(), outValues_, outIndices_);
	int32_t defined = 0;
	for (size_t i = 0; i < attrs_.size(); ++i) {
		SelectedValue value = xmlValue(xml_, attrs_[i]);
		// without theme, only resource references can be resolved
		if (value.value.dataType != TYPE_NULL) {
			assetManager_.resolveReference(value);
		}
		const uint32_t source = isUndefined(value) || xml_ == nullptr ? 0 : xml_->getSourceResId();
		if (writeValue(value, source, &outValues_[i * STYLE_NUM_ENTRIES])) {
			outIndices_[++defined] = static_cast<int32_t>(i);
		}
	}
	outIndices_[0] = defined;
	return true;
}
