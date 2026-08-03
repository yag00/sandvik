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

#include <jni/jni.h>

#include "system/logger.hpp"

extern "C" {

	JNIEXPORT void JNICALL Java_android_system_OsConstants_initConstants(JNIEnv* env, jclass clazz) {
		// Log for debugging purposes
		logger.debug("Initializing constants");

		// auto setInt = [&](const char* name, jint value) {
		// 	jfieldID field = env->GetStaticFieldID(clazz, name, "I");

		// 	if (field == nullptr) {
		// 		printf("OsConstants: missing field %s\n", name);
		// 		return;
		// 	}

		// 	env->SetStaticIntField(clazz, field, value);
		// };

		// // Address families
		// setInt("AF_UNIX", 1);
		// setInt("AF_INET", 2);
		// setInt("AF_INET6", 10);

		// // Socket types
		// setInt("SOCK_STREAM", 1);
		// setInt("SOCK_DGRAM", 2);
		// setInt("SOCK_RAW", 3);

		// // Protocols
		// setInt("IPPROTO_IP", 0);
		// setInt("IPPROTO_TCP", 6);
		// setInt("IPPROTO_UDP", 17);

		// // open()
		// setInt("O_RDONLY", 0);
		// setInt("O_WRONLY", 1);
		// setInt("O_RDWR", 2);

		// // seek()
		// setInt("SEEK_SET", 0);
		// setInt("SEEK_CUR", 1);
		// setInt("SEEK_END", 2);

		// // standard fds
		// setInt("STDIN_FILENO", 0);
		// setInt("STDOUT_FILENO", 1);
		// setInt("STDERR_FILENO", 2);

		// // poll()
		// setInt("POLLIN", 0x001);
		// setInt("POLLPRI", 0x002);
		// setInt("POLLOUT", 0x004);
	}
}  // extern "C"