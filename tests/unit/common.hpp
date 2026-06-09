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
#ifndef __COMMON_TEST__
#define __COMMON_TEST__

#include <vm.hpp>

inline void initializeVmRuntime(sandvik::Vm& vm) {
	// Load the runtime libraries
	vm.loadRt("../android-12.0.0-bin/core-oj.dex.jar");
	vm.loadRt("../android-12.0.0-bin/core-libart.dex.jar");
	vm.loadRt("../android-12.0.0-bin/icu-stubs.dex.jar");
}

#endif // __COMMON_TEST__