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

#ifndef __ANDROID_GRAPHICS_FONTS_NATIVEFONT_HPP__
#define __ANDROID_GRAPHICS_FONTS_NATIVEFONT_HPP__

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

// Font-less stand-in for minikin: fonts and families only keep what the Java side reads back
namespace sandvik::fonts {
	struct NativeFont {
			std::string path;
			std::string localeList;
			int weight = 400;
			bool italic = false;
			int ttcIndex = 0;
			std::vector<std::pair<int, float>> axes;  // OpenType axis tag, value
			void* buffer = nullptr;
			int64_t bufferSize = 0;
			int sourceId = 0;
	};

	struct NativeFontFamily {
			std::vector<NativeFont> fonts;
			std::string langTags;
			int variant = 0;
	};

	template <typename T> T* fromHandle(int64_t handle_) {
		return reinterpret_cast<T*>(static_cast<uintptr_t>(handle_));
	}

	template <typename T> int64_t toHandle(T* ptr_) {
		return static_cast<int64_t>(reinterpret_cast<uintptr_t>(ptr_));
	}
}  // namespace sandvik::fonts

#endif  // __ANDROID_GRAPHICS_FONTS_NATIVEFONT_HPP__
