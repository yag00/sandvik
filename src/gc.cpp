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

#include "gc.hpp"

#include "monitor.hpp"
#include "object.hpp"

using namespace sandvik;

void GC::release() {
	_objects.clear();
}

void GC::collect() {
	// For now, do nothing.
}

void GC::track(std::unique_ptr<Object> obj_) {
	_objects.push_back(std::move(obj_));
}
