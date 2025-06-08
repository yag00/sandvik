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

#include "version.hpp"

#include <string_view>
#include <version.in.hpp>

using namespace sandvik;

const std::string version::getBuildDate() {
#ifdef __TIMESTAMP__
	return std::string(__TIMESTAMP__);
#else
	return "";
#endif
}

const std::string version::getVersion() {
	return std::string(__Version);
}

const std::string version::getCommit() {
	return std::string(__GitHash);
}

const std::string version::getShortCommit() {
	return getCommit().substr(0, 7);
}
