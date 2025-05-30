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

#include "env_var.hpp"

#include <fmt/format.h>

#include <cstdlib>

using namespace sandvik::system;

std::string env::get(const std::string& name_) {
	const char* val = std::getenv(name_.c_str());
	if (val) return std::string(val);
	return "";
}

void env::set(const std::string& name_, const std::string& value_) {
	if (setenv(name_.c_str(), value_.c_str(), 1) != 0) throw std::runtime_error(fmt::format("Can't set {} environment variable!", name_));
}
