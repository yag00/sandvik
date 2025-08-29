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

#ifndef __RTLD_LOADER_HPP__
#define __RTLD_LOADER_HPP__

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace sandvik {
	class Dex;
	namespace rtld {
		void load(const std::string& path_, std::vector<std::unique_ptr<Dex>>& dexs_);
	}
}  // namespace sandvik
#endif  // __RTLD_LOADER_HPP__