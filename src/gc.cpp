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
#include "system/logger.hpp"
#include "vm.hpp"

using namespace sandvik;

void GC::manageVm(Vm* vm_) {
	_vms.push_back(vm_);
}

void GC::release() {
	_objects.clear();
}

void GC::collect() {
	// Stop-the-world: suspend all application threads
	for (auto& vm : _vms) {
		vm->suspend();
	}
	logger.fdebug("GC: Starting garbage collection cycle... ({} objects)", _objects.size());

	// Mark reachable objects:
	//  scan each thread's stacks/frames
	//  scan static fields in loaded classes
	//  @todo : scan thread's local JNI handles table
	//  @todo scan global JNI handles
	for (auto& vm : _vms) {
		vm->visitReferences([](Object* obj) { obj->setMarked(true); });
	}

	// Sweeping: free unmarked objects
	_objects.erase(std::remove_if(_objects.begin(), _objects.end(), [](const std::unique_ptr<Object>& obj) { return !obj->isMarked(); }), _objects.end());
	// Clear marks for next GC for live objects
	for (auto& obj : _objects) {
		obj->setMarked(false);
	}

	logger.fdebug("GC: {} live objects", _objects.size());
	// Resume the world
	for (auto& vm : _vms) {
		vm->resume();
	}
}

void GC::track(std::unique_ptr<Object> obj_) {
	_objects.push_back(std::move(obj_));
}
