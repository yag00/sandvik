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

#include "monitor.hpp"

using namespace sandvik;

void Monitor::enter() {
	std::unique_lock<std::mutex> lock(_mutex);
	// Wait until the monitor is free
	_condition.wait(lock, [this]() { return _owner == std::thread::id(); });
	_owner = std::this_thread::get_id();
}
void Monitor::exit() {
	std::unique_lock<std::mutex> lock(_mutex);
	if (_owner != std::this_thread::get_id()) {
		throw std::runtime_error("Cannot unlock an object not owned by the current thread.");
	}
	_owner = std::thread::id();  // Reset monitor ownership
	_condition.notify_one();
}
void Monitor::check() const {
	// Block until the current thread either owns the monitor or the monitor is free.
	// We don't take ownership here; we only wait until it's safe for the caller to proceed.
	while (true) {
		{
			std::unique_lock<std::mutex> lock(_mutex);
			if (_owner == std::thread::id() || _owner == std::this_thread::get_id()) {
				return;
			}
		}
		// Yield to avoid tight spinning while another thread holds the monitor.
		std::this_thread::yield();
	}
}