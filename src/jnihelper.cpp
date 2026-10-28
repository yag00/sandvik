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
#include "jnihelper.hpp"

#include <fmt/format.h>

#include <cctype>
#include <cstdint>
#include <string>

using namespace sandvik;

std::string JNIHelper::mangleMethodSignature(const std::string& sig) {
	std::string result;

	bool inParams = false;

	for (char c : sig) {
		if (c == '(') {
			inParams = true;
			continue;  // skip '('
		}
		if (c == ')') {
			break;  // stop after params
		}

		if (inParams) {
			switch (c) {
				case '_':
					result += "_1";
					break;
				case ';':
					result += "_2";
					break;
				case '[':
					result += "_3";
					break;
				case '/':
					result += "_";
					break;
				default:
					result.push_back(c);
					break;
			}
		}
	}
	return result;
}

std::string JNIHelper::mangleName(const std::string& name) {
	std::string result;
	for (size_t i = 0; i < name.size(); ++i) {
		const auto c = static_cast<unsigned char>(name[i]);
		if (c == '.' || c == '/') {
			result += '_';
		} else if (c == '_') {
			result += "_1";
		} else if (c == ';') {
			result += "_2";
		} else if (c == '[') {
			result += "_3";
		} else if (c < 0x80 && std::isalnum(c)) {
			result.push_back((char)c);
		} else {
			// decode one UTF-8 sequence, then escape it as UTF-16 code unit(s)
			uint32_t codepoint = c;
			int extra = 0;
			if (c >= 0xf0) {
				codepoint = c & 0x07;
				extra = 3;
			} else if (c >= 0xe0) {
				codepoint = c & 0x0f;
				extra = 2;
			} else if (c >= 0xc0) {
				codepoint = c & 0x1f;
				extra = 1;
			}
			for (; extra > 0 && i + 1 < name.size(); --extra) {
				codepoint = (codepoint << 6) | (static_cast<unsigned char>(name[++i]) & 0x3f);
			}
			if (codepoint >= 0x10000) {
				codepoint -= 0x10000;
				result += fmt::format("_0{:04x}_0{:04x}", 0xd800 + (codepoint >> 10), 0xdc00 + (codepoint & 0x3ff));
			} else {
				result += fmt::format("_0{:04x}", codepoint);
			}
		}
	}
	return result;
}
