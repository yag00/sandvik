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

#include <string.h>
#include <unistd.h>

#include <fmt/format.h>
#include <jni/jni.h>

#include <iostream>
#include <stdexcept>

#include "exceptions.hpp"
#include "native/native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

namespace {
	void __PrintStream__write(int fd, const std::string& s) {
		switch (fd) {
			case 1:
				std::cout << s;
				std::cout.flush();
				break;
			case 2:
				std::cerr << s;
				std::cerr.flush();
				break;
			default:
				ssize_t written = write(fd, s.c_str(), s.size());
				if (written == -1) {
					logger.fwarning("Failed to write {} to file descriptor {}", s, fd);
				}
				break;
		}
	}
}  // namespace

extern "C" {
	JNIEXPORT void JNICALL Java_java_io_PrintStream_print__Ljava_lang_String_2(JNIEnv* env, jobject obj, jstring str) {
		auto this_ptr = sandvik::native::getObject(obj);
		auto fileObj = this_ptr->getField("file");
		int fd = fileObj->getValue();
		auto strobj = sandvik::native::getString(str);
		const auto s = strobj->str();
		__PrintStream__write(fd, s);
	}
	JNIEXPORT void JNICALL Java_java_io_PrintStream_println__Ljava_lang_String_2(JNIEnv* env, jobject obj, jstring str) {
		auto this_ptr = sandvik::native::getObject(obj);
		auto fileObj = this_ptr->getField("file");
		int fd = fileObj->getValue();
		auto strobj = sandvik::native::getString(str);
		auto s = strobj->str() + "\n";
		__PrintStream__write(fd, s);
	}
	JNIEXPORT void JNICALL Java_java_io_PrintStream_println__(JNIEnv* env, jobject obj) {
		auto this_ptr = sandvik::native::getObject(obj);
		auto fileObj = this_ptr->getField("file");
		int fd = fileObj->getValue();
		__PrintStream__write(fd, "\n");
	}
	JNIEXPORT void JNICALL Java_java_io_PrintStream_println__Ljava_lang_Object_2(JNIEnv* env, jobject obj, jobject value) {
		auto this_ptr = sandvik::native::getObject(obj);
		auto fileObj = this_ptr->getField("file");
		int fd = fileObj->getValue();

		std::string s;
		if (value == nullptr) {
			s = "null";
		} else {
			auto o = sandvik::native::getObject(value);
			// @todo call toString() method of the object
			s = fmt::format("<{}>->toString() not implemented", o->debug());
			logger.fwarning("PrintStream.println(Object) toString() not implemented --> {}", o->debug());
		}
		__PrintStream__write(fd, s + "\n");
	}
}
