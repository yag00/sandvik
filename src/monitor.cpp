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
	std::unique_lock lock(_mutex);
	auto self = std::this_thread::get_id();

	// Reentrant acquire
	if (_owner == self) {
		_recursion++;
		return;
	}

	// Wait until monitor is free
	_condition.wait(lock, [this]() { return _owner == std::thread::id(); });

	_owner = self;
	_recursion = 1;
}

void Monitor::exit() {
	std::unique_lock lock(_mutex);
	auto self = std::this_thread::get_id();

	if (_owner != self) {
		throw std::runtime_error("IllegalMonitorStateException: current thread does not own the monitor");
	}

	_recursion--;

	if (_recursion == 0) {
		_owner = std::thread::id();
		_condition.notify_one();
	}
}

void Monitor::check() const {
	// Block until the current thread either owns the monitor or the monitor is free.
	// We don't take ownership here; we only wait until it's safe for the caller to proceed.
	while (true) {
		{
			std::unique_lock lock(_mutex);
			if (_owner == std::thread::id() || _owner == std::this_thread::get_id()) {
				return;
			}
		}
		// Yield to avoid tight spinning while another thread holds the monitor.
		std::this_thread::yield();
	}
}

bool Monitor::wait(uint64_t timeout_ms) {
	std::unique_lock<std::mutex> lock(_mutex);
	auto self = std::this_thread::get_id();

	// must own the monitor
	if (_owner != self) {
		throw std::runtime_error("wait() IllegalMonitorStateException");
	}

	// save recursion depth
	uint32_t saved_recursion = _recursion;

	// fully release the monitor
	_owner = std::thread::id();
	_recursion = 0;

	// wake one entering thread (entry set)
	_condition.notify_one();

	bool timed_out = false;

	// enter wait set
	if (timeout_ms == 0) {
		_wait_condition.wait(lock);
	} else {
		auto dur = std::chrono::milliseconds(timeout_ms);
		timed_out = (_wait_condition.wait_for(lock, dur) == std::cv_status::timeout);
	}

	// re-acquire the monitor (entry set)
	_condition.wait(lock, [this]() { return _owner == std::thread::id(); });

	// restore ownership + recursion
	_owner = self;
	_recursion = saved_recursion;

	return !timed_out;
}

void Monitor::notify() {
	std::unique_lock lock(_mutex);
	_wait_condition.notify_one();
}

void Monitor::notifyAll() {
	std::unique_lock lock(_mutex);
	_wait_condition.notify_all();
}
