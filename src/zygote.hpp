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

#ifndef __ZYGOTE_HPP__
#define __ZYGOTE_HPP__

#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>

/** @brief sandvik : project namespace */
namespace sandvik {
	/** @brief Sets up the Zygote Unix socket and exposes its file descriptor via ANDROID_SOCKET_zygote. */
	void setupZygoteSocket();
}  // namespace sandvik

#endif  // __ZYGOTE_HPP__