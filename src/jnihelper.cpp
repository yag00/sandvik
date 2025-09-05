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
