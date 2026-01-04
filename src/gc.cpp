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

#include <algorithm>

#include "monitor.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "vm.hpp"

using namespace sandvik;

GC::GC() : Thread("GC") {
}

GC::~GC() {
	_done.store(true);
	_gcRequested.store(true);
	_cv.notify_all();
}

void GC::onStart() {
	_done.store(false);
}

void GC::loop() {
	{
		std::unique_lock lock(_mtx);
		// Wait until someone requests a GC
		_cv.wait(lock, [this] { return _gcRequested.load(); });
		_gcRequested.store(false);
		if (_done.load()) {
			return;
		}
	}
	collect();
}

bool GC::done() {
	return _done.load();
}

uint64_t GC::getTrackedObjectCount() const {
	return _objects.size();
}

uint64_t GC::getLimit() const {
	return _limit;
}

void GC::setLimit(uint64_t limit_) {
	_limit = limit_;
}

void GC::manageVm(Vm* vm_) {
	bool startThread = _vms.empty();
	_vms.push_back(vm_);
	if (startThread && (getState() == ThreadState::NotStarted || getState() == ThreadState::Stopped)) {
		run();
	}
}

void GC::unmanageVm(Vm* vm_) {
	std::erase(_vms, vm_);
	if (_vms.empty()) {
		_done.store(true);
		_gcRequested.store(true);
		_cv.notify_all();
	}
}

void GC::release() {
	_objects.clear();
}

void GC::requestCollect() {
	std::unique_lock lock(_mtx);
	_gcRequested.store(true);
	_cv.notify_all();
	std::this_thread::yield();
}

uint64_t GC::getGcCycles() const {
	return _cycles.load();
}

void GC::collect() {
	// Stop-the-world: suspend all application threads
	for (auto& vm : _vms) {
		vm->suspend();
	}
	logger.fdebug("GC: Starting garbage collection cycle... ({} objects)", _objects.size());

	// Mark reachable objects:
	Object::makeNull()->setMarked(true);  // ensure null object is always marked
	//  scan each thread's stacks/frames
	//  scan static fields in loaded classes
	//  @todo : scan thread's local JNI handles table
	//  @todo scan global JNI handles
	for (auto& vm : _vms) {
		vm->visitReferences([](Object* obj) { obj->setMarked(true); });
	}

	// Sweeping: free unmarked objects
	std::erase_if(_objects, [](const std::unique_ptr<Object>& obj) { return !obj->isMarked(); });
	// Clear marks for next GC for live objects
	for (const auto& obj : _objects) {
		obj->setMarked(false);
	}

	logger.fdebug("GC: {} live objects", _objects.size());
	// Resume the world
	for (auto& vm : _vms) {
		vm->resume();
	}
	_cycles.fetch_add(1);
}

void GC::track(std::unique_ptr<Object> obj_) {
	if (_objects.size() > _limit) {
		requestCollect();
	}
	std::unique_lock lock(_mtx);
	_objects.push_back(std::move(obj_));
}
