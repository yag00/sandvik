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

#ifndef __RESOURCES_CONFIG_HPP__
#define __RESOURCES_CONFIG_HPP__

#include <cstdint>
#include <string>

#include "resources/resource_types.hpp"

namespace sandvik::resources {
	/**
	 * @struct ResConfig
	 * @brief Resource configuration qualifiers (androidfw ResTable_config), in its binary layout.
	 *
	 * Used both for the configuration of a resource variant (values-fr, drawable-xhdpi...) and for the configuration
	 * of the device the resources are requested for.
	 */
	struct ResConfig {
			// orientation
			static constexpr uint8_t ORIENTATION_ANY = 0;
			// density
			static constexpr uint16_t DENSITY_DEFAULT = 0;
			static constexpr uint16_t DENSITY_MEDIUM = 160;
			static constexpr uint16_t DENSITY_ANY = 0xfffe;
			static constexpr uint16_t DENSITY_NONE = 0xffff;
			// screenLayout
			static constexpr uint8_t MASK_SCREENSIZE = 0x0f;
			static constexpr uint8_t MASK_SCREENLONG = 0x30;
			static constexpr uint8_t MASK_LAYOUTDIR = 0xc0;
			// screenLayout2
			static constexpr uint8_t MASK_SCREENROUND = 0x03;
			// colorMode
			static constexpr uint8_t MASK_WIDE_COLOR_GAMUT = 0x03;
			static constexpr uint8_t MASK_HDR = 0x0c;
			// uiMode
			static constexpr uint8_t MASK_UI_MODE_TYPE = 0x0f;
			static constexpr uint8_t MASK_UI_MODE_NIGHT = 0x30;
			// inputFlags
			static constexpr uint8_t MASK_KEYSHIDDEN = 0x03;
			static constexpr uint8_t KEYSHIDDEN_NO = 0x01;
			static constexpr uint8_t KEYSHIDDEN_YES = 0x02;
			static constexpr uint8_t KEYSHIDDEN_SOFT = 0x03;
			static constexpr uint8_t MASK_NAVHIDDEN = 0x0c;

			uint32_t size = sizeof(ResConfig);
			uint16_t mcc = 0;
			uint16_t mnc = 0;
			char language[2] = {0, 0};
			char country[2] = {0, 0};
			uint8_t orientation = 0;
			uint8_t touchscreen = 0;
			uint16_t density = 0;
			uint8_t keyboard = 0;
			uint8_t navigation = 0;
			uint8_t inputFlags = 0;
			uint8_t inputPad0 = 0;
			uint16_t screenWidth = 0;
			uint16_t screenHeight = 0;
			uint16_t sdkVersion = 0;
			uint16_t minorVersion = 0;
			uint8_t screenLayout = 0;
			uint8_t uiMode = 0;
			uint16_t smallestScreenWidthDp = 0;
			uint16_t screenWidthDp = 0;
			uint16_t screenHeightDp = 0;
			char localeScript[4] = {0, 0, 0, 0};
			char localeVariant[8] = {0, 0, 0, 0, 0, 0, 0, 0};
			uint8_t screenLayout2 = 0;
			uint8_t colorMode = 0;
			uint16_t screenConfigPad2 = 0;
			bool localeScriptWasComputed = false;
			char localeNumberingSystem[8] = {0, 0, 0, 0, 0, 0, 0, 0};

			/** @brief Reads a configuration stored in a ResTable_type (its size field gives the stored length) */
			static ResConfig read(const ByteView& data_, size_t offset_);

			/** @brief Sets language, country and script from a BCP 47 tag ("en-US", "zh-Hant-TW"...) */
			void setBcp47Locale(const std::string& tag_);
			/** @brief Language tag of the locale qualifiers ("en-US"), empty if no locale */
			std::string getBcp47Locale() const;

			/** @brief True if a resource with this configuration can be used on a device with configuration settings_ */
			bool match(const ResConfig& settings_) const;
			/** @brief True if this configuration is a better match than o_ for the requested configuration.
			 * Both configurations must match the requested one. */
			bool isBetterThan(const ResConfig& o_, const ResConfig& requested_) const;
			/** @brief Qualifiers string ("fr-rFR-xhdpi-v21"), for logs and Configuration lists */
			std::string toString() const;

		private:
			bool isLocaleBetterThan(const ResConfig& o_, const ResConfig& requested_) const;
	};
	static_assert(sizeof(ResConfig) == 64, "ResConfig must keep the ResTable_config binary layout");
}  // namespace sandvik::resources

#endif  // __RESOURCES_CONFIG_HPP__
