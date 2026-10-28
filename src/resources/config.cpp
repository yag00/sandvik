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

#include "resources/config.hpp"

#include <fmt/format.h>

#include <cctype>
#include <sstream>
#include <utility>
#include <vector>

using namespace sandvik::resources;

namespace {
	// 2 letters codes are stored as is, 3 letters codes are packed on 2 bytes (high bit set), see androidfw packLanguageOrRegion
	void packCode(const std::string& code_, char base_, char out_[2]) {
		if (code_.size() == 2) {
			out_[0] = code_[0];
			out_[1] = code_[1];
		} else if (code_.size() == 3) {
			const int c0 = code_[0] - base_;
			const int c1 = code_[1] - base_;
			const int c2 = code_[2] - base_;
			out_[0] = static_cast<char>(0x80 | (c2 << 2) | (c1 >> 3));
			out_[1] = static_cast<char>(((c1 & 0x07) << 5) | c0);
		} else {
			out_[0] = 0;
			out_[1] = 0;
		}
	}

	std::string unpackCode(const char in_[2], char base_) {
		if (in_[0] == 0) {
			return "";
		}
		if (static_cast<unsigned char>(in_[0]) & 0x80) {
			const auto b0 = static_cast<unsigned char>(in_[0]);
			const auto b1 = static_cast<unsigned char>(in_[1]);
			std::string result(3, '\0');
			result[0] = static_cast<char>(base_ + (b1 & 0x1f));
			result[1] = static_cast<char>(base_ + (((b0 & 0x03) << 3) | (b1 >> 5)));
			result[2] = static_cast<char>(base_ + ((b0 >> 2) & 0x1f));
			return result;
		}
		return std::string(in_, 2);
	}

	bool sameCode(const char a_[2], const char b_[2]) {
		return a_[0] == b_[0] && a_[1] == b_[1];
	}

	/** @brief "A set qualifier that differs from the other one wins, if the device requested this qualifier" */
	template <typename T>
	bool differs(T a_, T b_) {
		return a_ != b_;
	}
}  // namespace

ResConfig ResConfig::read(const ByteView& data_, size_t offset_) {
	const auto storedSize = data_.read<uint32_t>(offset_);
	ResConfig config = data_.readPartial<ResConfig>(offset_, storedSize);
	config.size = sizeof(ResConfig);
	return config;
}

void ResConfig::setBcp47Locale(const std::string& tag_) {
	std::memset(language, 0, sizeof(language));
	std::memset(country, 0, sizeof(country));
	std::memset(localeScript, 0, sizeof(localeScript));
	std::memset(localeVariant, 0, sizeof(localeVariant));
	localeScriptWasComputed = false;

	std::vector<std::string> parts;
	std::stringstream stream(tag_);
	std::string part;
	while (std::getline(stream, part, '-')) {
		if (!part.empty()) {
			parts.push_back(part);
		}
	}
	if (parts.empty() || parts[0] == "und") {
		return;
	}
	for (auto& c : parts[0]) {
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	}
	packCode(parts[0], 'a', language);
	for (size_t i = 1; i < parts.size(); ++i) {
		auto& subtag = parts[i];
		if (subtag.size() == 4 && std::isalpha(static_cast<unsigned char>(subtag[0]))) {
			// script: Titlecase (Latn, Hant)
			for (size_t j = 0; j < 4; ++j) {
				localeScript[j] =
				    static_cast<char>(j == 0 ? std::toupper(static_cast<unsigned char>(subtag[j])) : std::tolower(static_cast<unsigned char>(subtag[j])));
			}
		} else if (subtag.size() == 2 || (subtag.size() == 3 && std::isdigit(static_cast<unsigned char>(subtag[0])))) {
			for (auto& c : subtag) {
				c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
			}
			packCode(subtag, '0', country);
		} else if (subtag.size() >= 5 && subtag.size() <= 8) {
			std::memcpy(localeVariant, subtag.data(), subtag.size());
		}
	}
}

std::string ResConfig::getBcp47Locale() const {
	std::string result = unpackCode(language, 'a');
	if (result.empty()) {
		return result;
	}
	if (localeScript[0] != 0 && !localeScriptWasComputed) {
		result += "-" + std::string(localeScript, 4);
	}
	if (country[0] != 0) {
		result += "-" + unpackCode(country, '0');
	}
	if (localeVariant[0] != 0) {
		result += "-" + std::string(localeVariant, strnlen(localeVariant, sizeof(localeVariant)));
	}
	return result;
}

bool ResConfig::match(const ResConfig& settings_) const {
	// imsi
	if (mcc != 0 && mcc != settings_.mcc) {
		return false;
	}
	if (mnc != 0 && mnc != settings_.mnc) {
		return false;
	}
	// locale: a set qualifier must be the requested one
	if (language[0] != 0 && !sameCode(language, settings_.language)) {
		return false;
	}
	if (country[0] != 0 && !sameCode(country, settings_.country)) {
		return false;
	}
	if (localeScript[0] != 0 && settings_.localeScript[0] != 0 && std::memcmp(localeScript, settings_.localeScript, sizeof(localeScript)) != 0) {
		return false;
	}
	if (localeVariant[0] != 0 && std::memcmp(localeVariant, settings_.localeVariant, sizeof(localeVariant)) != 0) {
		return false;
	}
	// screenConfig
	const uint8_t layoutDir = screenLayout & MASK_LAYOUTDIR;
	if (layoutDir != 0 && layoutDir != (settings_.screenLayout & MASK_LAYOUTDIR)) {
		return false;
	}
	// a resource for a screen size is usable on larger screens
	const uint8_t screenSize = screenLayout & MASK_SCREENSIZE;
	if (screenSize != 0 && screenSize > (settings_.screenLayout & MASK_SCREENSIZE)) {
		return false;
	}
	const uint8_t screenLong = screenLayout & MASK_SCREENLONG;
	if (screenLong != 0 && screenLong != (settings_.screenLayout & MASK_SCREENLONG)) {
		return false;
	}
	const uint8_t uiModeType = uiMode & MASK_UI_MODE_TYPE;
	if (uiModeType != 0 && uiModeType != (settings_.uiMode & MASK_UI_MODE_TYPE)) {
		return false;
	}
	const uint8_t uiModeNight = uiMode & MASK_UI_MODE_NIGHT;
	if (uiModeNight != 0 && uiModeNight != (settings_.uiMode & MASK_UI_MODE_NIGHT)) {
		return false;
	}
	// dp sizes are minimums
	if (smallestScreenWidthDp != 0 && smallestScreenWidthDp > settings_.smallestScreenWidthDp) {
		return false;
	}
	if (screenWidthDp != 0 && screenWidthDp > settings_.screenWidthDp) {
		return false;
	}
	if (screenHeightDp != 0 && screenHeightDp > settings_.screenHeightDp) {
		return false;
	}
	// screenLayout2 / colorMode
	const uint8_t round = screenLayout2 & MASK_SCREENROUND;
	if (round != 0 && round != (settings_.screenLayout2 & MASK_SCREENROUND)) {
		return false;
	}
	const uint8_t wideGamut = colorMode & MASK_WIDE_COLOR_GAMUT;
	if (wideGamut != 0 && wideGamut != (settings_.colorMode & MASK_WIDE_COLOR_GAMUT)) {
		return false;
	}
	const uint8_t hdr = colorMode & MASK_HDR;
	if (hdr != 0 && hdr != (settings_.colorMode & MASK_HDR)) {
		return false;
	}
	// screenType: the density is never a reason to reject a resource, it is only used to pick the best one
	if (orientation != 0 && orientation != settings_.orientation) {
		return false;
	}
	if (touchscreen != 0 && touchscreen != settings_.touchscreen) {
		return false;
	}
	// input
	const uint8_t keysHidden = inputFlags & MASK_KEYSHIDDEN;
	const uint8_t settingsKeysHidden = settings_.inputFlags & MASK_KEYSHIDDEN;
	if (keysHidden != 0 && keysHidden != settingsKeysHidden) {
		// keysexposed also matches a device with only a soft keyboard exposed
		if (keysHidden != KEYSHIDDEN_NO || settingsKeysHidden != KEYSHIDDEN_SOFT) {
			return false;
		}
	}
	const uint8_t navHidden = inputFlags & MASK_NAVHIDDEN;
	if (navHidden != 0 && navHidden != (settings_.inputFlags & MASK_NAVHIDDEN)) {
		return false;
	}
	if (keyboard != 0 && keyboard != settings_.keyboard) {
		return false;
	}
	if (navigation != 0 && navigation != settings_.navigation) {
		return false;
	}
	// screen size in pixels and platform version are minimums
	if (screenWidth != 0 && screenWidth > settings_.screenWidth) {
		return false;
	}
	if (screenHeight != 0 && screenHeight > settings_.screenHeight) {
		return false;
	}
	if (sdkVersion != 0 && sdkVersion > settings_.sdkVersion) {
		return false;
	}
	if (minorVersion != 0 && minorVersion != settings_.minorVersion) {
		return false;
	}
	return true;
}

bool ResConfig::isLocaleBetterThan(const ResConfig& o_, const ResConfig& requested_) const {
	if (requested_.language[0] == 0) {
		// the device has no locale: no locale specific resource is preferred
		return false;
	}
	if (!sameCode(language, o_.language)) {
		// both matched, so the one setting the language is the requested language
		return language[0] != 0;
	}
	const bool scriptDiffers = std::memcmp(localeScript, o_.localeScript, sizeof(localeScript)) != 0;
	if (scriptDiffers && requested_.localeScript[0] != 0) {
		return std::memcmp(localeScript, requested_.localeScript, sizeof(localeScript)) == 0;
	}
	if (!sameCode(country, o_.country) && requested_.country[0] != 0) {
		return country[0] != 0;
	}
	if (std::memcmp(localeVariant, o_.localeVariant, sizeof(localeVariant)) != 0 && requested_.localeVariant[0] != 0) {
		return localeVariant[0] != 0;
	}
	return false;
}

bool ResConfig::isBetterThan(const ResConfig& o_, const ResConfig& requested_) const {
	// Same priority order as the qualifiers in a resource directory name (androidfw ResTable_config::isBetterThan)
	if (differs(mcc, o_.mcc) && requested_.mcc != 0) {
		return mcc != 0;
	}
	if (differs(mnc, o_.mnc) && requested_.mnc != 0) {
		return mnc != 0;
	}
	const bool localeDiffers = !sameCode(language, o_.language) || !sameCode(country, o_.country) ||
	                           std::memcmp(localeScript, o_.localeScript, sizeof(localeScript)) != 0 ||
	                           std::memcmp(localeVariant, o_.localeVariant, sizeof(localeVariant)) != 0;
	if (localeDiffers) {
		if (isLocaleBetterThan(o_, requested_)) {
			return true;
		}
		if (o_.isLocaleBetterThan(*this, requested_)) {
			return false;
		}
	}
	const uint8_t layoutDir = screenLayout & MASK_LAYOUTDIR;
	const uint8_t oLayoutDir = o_.screenLayout & MASK_LAYOUTDIR;
	if (layoutDir != oLayoutDir && (requested_.screenLayout & MASK_LAYOUTDIR) != 0) {
		return layoutDir != 0;
	}
	// dp sizes: both are below the requested value, the closest (largest) wins
	if (smallestScreenWidthDp != o_.smallestScreenWidthDp && requested_.smallestScreenWidthDp != 0) {
		return smallestScreenWidthDp > o_.smallestScreenWidthDp;
	}
	if (screenWidthDp != o_.screenWidthDp && requested_.screenWidthDp != 0) {
		return screenWidthDp > o_.screenWidthDp;
	}
	if (screenHeightDp != o_.screenHeightDp && requested_.screenHeightDp != 0) {
		return screenHeightDp > o_.screenHeightDp;
	}
	const uint8_t screenSize = screenLayout & MASK_SCREENSIZE;
	const uint8_t oScreenSize = o_.screenLayout & MASK_SCREENSIZE;
	if (screenSize != oScreenSize && (requested_.screenLayout & MASK_SCREENSIZE) != 0) {
		return screenSize > oScreenSize;
	}
	const uint8_t screenLong = screenLayout & MASK_SCREENLONG;
	const uint8_t oScreenLong = o_.screenLayout & MASK_SCREENLONG;
	if (screenLong != oScreenLong && (requested_.screenLayout & MASK_SCREENLONG) != 0) {
		return screenLong != 0;
	}
	const uint8_t round = screenLayout2 & MASK_SCREENROUND;
	const uint8_t oRound = o_.screenLayout2 & MASK_SCREENROUND;
	if (round != oRound && (requested_.screenLayout2 & MASK_SCREENROUND) != 0) {
		return round != 0;
	}
	const uint8_t hdr = colorMode & MASK_HDR;
	const uint8_t oHdr = o_.colorMode & MASK_HDR;
	if (hdr != oHdr && (requested_.colorMode & MASK_HDR) != 0) {
		return hdr != 0;
	}
	const uint8_t wideGamut = colorMode & MASK_WIDE_COLOR_GAMUT;
	const uint8_t oWideGamut = o_.colorMode & MASK_WIDE_COLOR_GAMUT;
	if (wideGamut != oWideGamut && (requested_.colorMode & MASK_WIDE_COLOR_GAMUT) != 0) {
		return wideGamut != 0;
	}
	if (orientation != o_.orientation && requested_.orientation != 0) {
		return orientation != 0;
	}
	const uint8_t uiModeType = uiMode & MASK_UI_MODE_TYPE;
	const uint8_t oUiModeType = o_.uiMode & MASK_UI_MODE_TYPE;
	if (uiModeType != oUiModeType && (requested_.uiMode & MASK_UI_MODE_TYPE) != 0) {
		return uiModeType != 0;
	}
	const uint8_t uiModeNight = uiMode & MASK_UI_MODE_NIGHT;
	const uint8_t oUiModeNight = o_.uiMode & MASK_UI_MODE_NIGHT;
	if (uiModeNight != oUiModeNight && (requested_.uiMode & MASK_UI_MODE_NIGHT) != 0) {
		return uiModeNight != 0;
	}
	if (density != o_.density) {
		// no density means the default one (mdpi); "anydpi" always beats scaling a density bucket
		const int thisDensity = density != 0 ? density : DENSITY_MEDIUM;
		const int otherDensity = o_.density != 0 ? o_.density : DENSITY_MEDIUM;
		if (thisDensity == DENSITY_ANY) {
			return true;
		}
		if (otherDensity == DENSITY_ANY) {
			return false;
		}
		int requestedDensity = requested_.density != 0 ? requested_.density : DENSITY_MEDIUM;
		if (requested_.density == DENSITY_ANY) {
			requestedDensity = DENSITY_MEDIUM;
		}
		int h = thisDensity;
		int l = otherDensity;
		bool imBigger = true;
		if (l > h) {
			std::swap(h, l);
			imBigger = false;
		}
		if (requestedDensity >= h) {
			// requested density is higher than both: the higher one wins
			return imBigger;
		}
		if (l >= requestedDensity) {
			// requested density is lower than both: the lower one wins
			return !imBigger;
		}
		// in between: scaling down is considered twice as good as scaling up
		if (((2 * l) - requestedDensity) * h > requestedDensity * requestedDensity) {
			return !imBigger;
		}
		return imBigger;
	}
	if (touchscreen != o_.touchscreen && requested_.touchscreen != 0) {
		return touchscreen != 0;
	}
	const uint8_t keysHidden = inputFlags & MASK_KEYSHIDDEN;
	const uint8_t oKeysHidden = o_.inputFlags & MASK_KEYSHIDDEN;
	if (keysHidden != oKeysHidden && (requested_.inputFlags & MASK_KEYSHIDDEN) != 0) {
		if (keysHidden == 0) {
			return false;
		}
		if (oKeysHidden == 0) {
			return true;
		}
		// both set: the exact match wins over keysexposed matching a soft keyboard
		return keysHidden == (requested_.inputFlags & MASK_KEYSHIDDEN);
	}
	const uint8_t navHidden = inputFlags & MASK_NAVHIDDEN;
	const uint8_t oNavHidden = o_.inputFlags & MASK_NAVHIDDEN;
	if (navHidden != oNavHidden && (requested_.inputFlags & MASK_NAVHIDDEN) != 0) {
		return navHidden != 0;
	}
	if (keyboard != o_.keyboard && requested_.keyboard != 0) {
		return keyboard != 0;
	}
	if (navigation != o_.navigation && requested_.navigation != 0) {
		return navigation != 0;
	}
	if (screenWidth != o_.screenWidth && requested_.screenWidth != 0) {
		return screenWidth > o_.screenWidth;
	}
	if (screenHeight != o_.screenHeight && requested_.screenHeight != 0) {
		return screenHeight > o_.screenHeight;
	}
	if (sdkVersion != o_.sdkVersion && requested_.sdkVersion != 0) {
		return sdkVersion > o_.sdkVersion;
	}
	if (minorVersion != o_.minorVersion && requested_.minorVersion != 0) {
		return minorVersion != 0;
	}
	return false;
}

std::string ResConfig::toString() const {
	std::vector<std::string> qualifiers;
	if (mcc != 0) {
		qualifiers.push_back(fmt::format("mcc{}", mcc));
	}
	if (mnc != 0) {
		qualifiers.push_back(fmt::format("mnc{}", mnc));
	}
	if (auto locale = getBcp47Locale(); !locale.empty()) {
		qualifiers.push_back(locale);
	}
	switch (screenLayout & MASK_LAYOUTDIR) {
		case 0x40:
			qualifiers.push_back("ldltr");
			break;
		case 0x80:
			qualifiers.push_back("ldrtl");
			break;
		default:
			break;
	}
	if (smallestScreenWidthDp != 0) {
		qualifiers.push_back(fmt::format("sw{}dp", smallestScreenWidthDp));
	}
	if (screenWidthDp != 0) {
		qualifiers.push_back(fmt::format("w{}dp", screenWidthDp));
	}
	if (screenHeightDp != 0) {
		qualifiers.push_back(fmt::format("h{}dp", screenHeightDp));
	}
	static const char* sizes[] = {"", "small", "normal", "large", "xlarge"};
	if (const auto size = screenLayout & MASK_SCREENSIZE; size != 0 && size < 5) {
		qualifiers.push_back(sizes[size]);
	}
	if (orientation == 1) {
		qualifiers.push_back("port");
	} else if (orientation == 2) {
		qualifiers.push_back("land");
	}
	if (const auto night = uiMode & MASK_UI_MODE_NIGHT; night != 0) {
		qualifiers.push_back(night == 0x20 ? "night" : "notnight");
	}
	switch (density) {
		case 0:
			break;
		case 120:
			qualifiers.push_back("ldpi");
			break;
		case 160:
			qualifiers.push_back("mdpi");
			break;
		case 213:
			qualifiers.push_back("tvdpi");
			break;
		case 240:
			qualifiers.push_back("hdpi");
			break;
		case 320:
			qualifiers.push_back("xhdpi");
			break;
		case 480:
			qualifiers.push_back("xxhdpi");
			break;
		case 640:
			qualifiers.push_back("xxxhdpi");
			break;
		case DENSITY_ANY:
			qualifiers.push_back("anydpi");
			break;
		case DENSITY_NONE:
			qualifiers.push_back("nodpi");
			break;
		default:
			qualifiers.push_back(fmt::format("{}dpi", density));
			break;
	}
	if (sdkVersion != 0) {
		qualifiers.push_back(fmt::format("v{}", sdkVersion));
	}
	std::string result;
	for (const auto& qualifier : qualifiers) {
		result += (result.empty() ? "" : "-") + qualifier;
	}
	return result.empty() ? "default" : result;
}
