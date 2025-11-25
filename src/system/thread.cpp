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
#include "thread.hpp"

#include "system/logger.hpp"

using namespace sandvik;

Thread::Thread(const std::string& name_) : _name(name_) {
}
Thread::~Thread() {
	if (_thread.joinable()) {
		auto current = std::this_thread::get_id();
		if (_thread.get_id() != current) {
			try {
				_thread.join();
			} catch (const std::exception& ex) {
				logger.ferror("Failed to join thread '{}': {}", _name, ex.what());
			}
		} else {
			logger.fwarning("Destructor invoked from thread '{}' itself; detaching to avoid deadlock", _name);
			_thread.detach();
		}
	}
}

void Thread::run(bool wait_) {
	if (_state.load() == ThreadState::Running) {
		return;
	}
	if (_state.load() != ThreadState::Stopped && _state.load() != ThreadState::NotStarted) {
		throw std::runtime_error(fmt::format("Thread '{}' is in invalid state to be started {}", _name, (uint64_t)_state.load()));
	}
	if (_state.load() == ThreadState::Stopped) {
		// If a previous std::thread instance is still joinable, join it before creating a new thread.
		// Assigning to a joinable std::thread causes std::terminate().
		if (_thread.joinable()) {
			auto current = std::this_thread::get_id();
			if (_thread.get_id() != current) {
				try {
					_thread.join();
				} catch (const std::exception& ex) {
					logger.ferror("Failed to join previous thread '{}': {}", _name, ex.what());
				} catch (...) {
					logger.ferror("Failed to join previous thread '{}' : unknown exception", _name);
				}
			} else {
				// Shouldn't normally happen: attempting to join the same thread object from within that thread.
				// Detach to avoid deadlock / terminate.
				logger.fwarning("Attempting to restart thread '{}' from within the same thread; detaching previous thread object", _name);
				_thread.detach();
			}
		}
	}

	_state.store(ThreadState::Running);
	_thread = std::thread([this]() {
		auto id = std::this_thread::get_id();
		if (_name != "main") {
			logger.addThread(id, _name);
		}
		logger.fdebug("Starting thread '{}'", _name);
		while (_state.load() != ThreadState::Stopped && !done()) {
			if (_state.load() == ThreadState::SuspendedRequested) {
				std::unique_lock<std::mutex> lock(_mtx);
				_state.store(ThreadState::Suspended);  // Confirm suspension
				_cv.notify_all();                      // Notify that the thread is suspended
				_cv.wait(lock, [this]() { return _state.load() != ThreadState::Suspended; });
			}
			if (_state.load() == ThreadState::Stopped) {
				break;
			}
			loop();
		}
		_state.store(ThreadState::Stopped);
		logger.fdebug("End of thread '{}'", _name);
		logger.removeThread(id);
	});
	if (wait_ && _thread.joinable()) {
		_thread.join();
	}
}

void Thread::join() {
	if (_thread.joinable()) {
		_thread.join();
	}
}

bool Thread::isRunning() const {
	return _state.load() == ThreadState::Running;
}

Thread::ThreadState Thread::getState() const {
	return _state.load();
}

void Thread::suspend() {
	std::unique_lock<std::mutex> lock(_mtx);
	if (_state.load() == ThreadState::Running) {
		_state.store(ThreadState::SuspendedRequested);  // Request suspension
		_cv.wait(lock, [this]() { return _state.load() == ThreadState::Suspended; });
	}
}

void Thread::resume() {
	std::lock_guard<std::mutex> lock(_mtx);
	if (_state.load() == ThreadState::Suspended) {
		_state.store(ThreadState::Running);
		_cv.notify_all();  // Wake up the thread
	}
}

void Thread::stop() {
	_state.store(ThreadState::Stopped);
	_cv.notify_all();  // Wake up the thread
}
