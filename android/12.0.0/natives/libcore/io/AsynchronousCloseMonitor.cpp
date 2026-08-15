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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <jni/jni.h>

#include "system/logger.hpp"

using namespace sandvik;

extern "C" {
	JNIEXPORT void JNICALL Java_libcore_io_AsynchronousCloseMonitor_signalBlockedThreads(JNIEnv* env, jclass clazz, jobject fd) {
		// TODO: asynchronous close monitoring
		logger.fdebug("AsynchronousCloseMonitor.signalBlockedThreads called, but not implemented");
	}
}  // extern "C"
