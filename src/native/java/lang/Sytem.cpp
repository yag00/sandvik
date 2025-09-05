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

#include <fmt/format.h>
#include <jni/jni.h>

#include "class.hpp"
#include "classloader.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "native/native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "vm.hpp"

extern "C" {
	JNIEXPORT void JNICALL Java_java_lang_System_initializeStream(JNIEnv* env, jobject obj) {
		auto jenv = sandvik::native::getNativeInterface(env);
		sandvik::ClassLoader& classloader = jenv->getClassLoader();
		auto& systemClass = classloader.getOrLoad("java.lang.System");
		auto& printStreamClass = classloader.getOrLoad("java.io.PrintStream");
		// initialize System.out
		auto stream_stdout = sandvik::Object::make(printStreamClass);
		stream_stdout->setField("file", sandvik::Object::make(1));
		systemClass.getField("out").setObjectValue(stream_stdout);
		// initialize System.err
		auto stream_stderr = sandvik::Object::make(printStreamClass);
		stream_stderr->setField("file", sandvik::Object::make(2));
		systemClass.getField("err").setObjectValue(stream_stdout);
	}

	JNIEXPORT void JNICALL Java_java_lang_System_loadLibrary(JNIEnv* env, jobject obj, jstring libName) {
		auto jenv = sandvik::native::getNativeInterface(env);
		auto name = sandvik::native::getString(libName);
		jenv->getVm().loadLibrary(fmt::format("lib{}.so", name->str()));
	}
}