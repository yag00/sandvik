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

#include "zygote.hpp"

using namespace sandvik;

// Creates a Unix domain socket in Linux's abstract namespace, bound
// and listening, and exports its fd via the given environment
// variable name — mirroring how Android's init sets up
// ANDROID_SOCKET_<name> before launching zygote.
int createAbstractUnixSocket(const std::string& name_) {
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd < 0) {
		throw std::runtime_error("createAbstractUnixSocket(" + name_ + "): socket() failed: " + strerror(errno));
	}

	struct sockaddr_un addr{};
	addr.sun_family = AF_UNIX;
	addr.sun_path[0] = '\0';

	// The abstract name is not null-terminated: its length is carried explicitly via
	// addrLen, so it must fit entirely within sun_path (reject rather than silently
	// truncate, which would otherwise desync addrLen from what was actually copied).
	const size_t maxNameLen = sizeof(addr.sun_path) - 2;
	if (name_.size() > maxNameLen) {
		throw std::runtime_error("createAbstractUnixSocket(" + name_ + "): socket name too long");
	}
	std::memcpy(addr.sun_path + 1, name_.data(), name_.size());
	socklen_t addrLen = static_cast<socklen_t>(offsetof(struct sockaddr_un, sun_path) + 1 + name_.size());

	if (bind(fd, (struct sockaddr*)&addr, addrLen) < 0) {
		throw std::runtime_error("createAbstractUnixSocket(" + name_ + "): bind() failed: " + strerror(errno));
	}
	if (listen(fd, 8) < 0) {
		throw std::runtime_error("createAbstractUnixSocket(" + name_ + "): listen() failed: " + strerror(errno));
	}
	return fd;
}

// TEMPORARY STUB: creates a real Unix socket and exposes its fd via
// ANDROID_SOCKET_zygote so that ZygoteInit's startup check passes, but
// nothing ever accepts connections on it yet.
//
// This is NOT sufficient for the real zygote workflow: on real Android,
// ActivityManagerService (running in system_server, itself forked from
// zygote) connects to this socket and sends fork requests (uid/gid,
// class to instantiate, etc.), and the zygote responds with a real
// fork(2) to spawn each app process.
//
// TODO: before implementing system_server / ActivityManager support,
// decide with the team whether "forking" a process here means:
//   (a) a real OS-level fork(2) of the sandvik process (more faithful,
//       but risky given our C++ VM's threads/mutexes/GC state), or
//   (b) an in-process simulated "process" (a fresh isolated VM/thread
//       context reusing already-loaded classes, simpler to drive from
//       our synchronous interpreter).
// Whichever is chosen, this stub must be replaced with a real accept()
// loop and command handling (see ZygoteServer.runSelectLoop /
// ZygoteConnection.processOneCommand in AOSP for reference).
void sandvik::setupZygoteSocket() {
	int zygoteFd = createAbstractUnixSocket("zygote");
	setenv("ANDROID_SOCKET_zygote", std::to_string(zygoteFd).c_str(), 1);

	int usapFd = createAbstractUnixSocket("usap_pool_primary");
	setenv("ANDROID_SOCKET_usap_pool_primary", std::to_string(usapFd).c_str(), 1);
}