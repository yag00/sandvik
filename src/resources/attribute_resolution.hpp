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

#ifndef __RESOURCES_ATTRIBUTE_RESOLUTION_HPP__
#define __RESOURCES_ATTRIBUTE_RESOLUTION_HPP__

#include <cstdint>
#include <vector>

#include "resources/asset_manager.hpp"

/**
 * Attribute resolution behind Context.obtainStyledAttributes / Resources.obtainAttributes
 * (androidfw AttributeResolution). Results use the TypedArray layout: STYLE_NUM_ENTRIES ints per attribute
 * in outValues, and in outIndices the count of defined attributes followed by their indexes.
 */
namespace sandvik::resources {
	/** @brief TypedArray.STYLE_* */
	constexpr size_t STYLE_TYPE = 0;
	constexpr size_t STYLE_DATA = 1;
	constexpr size_t STYLE_ASSET_COOKIE = 2;
	constexpr size_t STYLE_RESOURCE_ID = 3;
	constexpr size_t STYLE_CHANGING_CONFIGURATIONS = 4;
	constexpr size_t STYLE_DENSITY = 5;
	constexpr size_t STYLE_SOURCE_RESOURCE_ID = 6;
	constexpr size_t STYLE_NUM_ENTRIES = 7;

	/** @brief Attributes of an XML element (AttributeSet of an inflated layout) */
	class XmlAttributes {
		public:
			virtual ~XmlAttributes() = default;
			virtual size_t getAttributeCount() const = 0;
			/** @brief Attribute resource id (android:text -> 0x0101014f), 0 if the attribute has none */
			virtual uint32_t getAttributeNameResId(size_t index_) const = 0;
			virtual ResValue getAttributeValue(size_t index_) const = 0;
			/** @brief style="@style/..." of the element, 0 if none */
			virtual uint32_t getStyleResId() const = 0;
			/** @brief Resource id of the XML file (layout), 0 if unknown */
			virtual uint32_t getSourceResId() const = 0;
	};

	/** @brief Values for attrs_: XML attributes, then XML style, then default style, then theme */
	void applyStyle(const Theme& theme_, const XmlAttributes* xml_, uint32_t defStyleAttr_, uint32_t defStyleRes_, const std::vector<uint32_t>& attrs_,
	                std::vector<int32_t>& outValues_, std::vector<int32_t>& outIndices_);
	/** @brief Values for attrs_: given attribute references (srcValues_), then default style, then theme */
	bool resolveAttrs(const Theme& theme_, uint32_t defStyleAttr_, uint32_t defStyleRes_, const std::vector<uint32_t>& srcValues_,
	                  const std::vector<uint32_t>& attrs_, std::vector<int32_t>& outValues_, std::vector<int32_t>& outIndices_);
	/** @brief Values for attrs_ from XML attributes only, without theme */
	bool retrieveAttributes(const AssetManager& assetManager_, const XmlAttributes* xml_, const std::vector<uint32_t>& attrs_, std::vector<int32_t>& outValues_,
	                        std::vector<int32_t>& outIndices_);
}  // namespace sandvik::resources

#endif  // __RESOURCES_ATTRIBUTE_RESOLUTION_HPP__
