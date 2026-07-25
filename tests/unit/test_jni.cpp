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
#include <gtest/gtest.h>
#include <fmt/core.h>
#include <fmt/ostream.h>

#include <vm.hpp>
#include <classloader.hpp>
#include <jni.hpp>
#include <system/sharedlibrary.hpp>
#include <system/logger.hpp>
#include "common.hpp"

using namespace sandvik;

TEST(JNI, JNI) {
	logger.setLevel(Logger::LogLevel::NONE);
	Vm vm;

	// Redirect stdout to output.txt
	FILE* file = freopen("test_jni.out", "w", stdout);
	ASSERT_NE(file, nullptr) << "Failed to redirect stdout";

	initializeVmRuntime(vm);
	vm.loadRt();
	vm.loadDex("../tests/java/jni/classes.dex");
	vm.run("TestJNI", {});

	// Compare output with reference file
	std::ifstream outputFile("test_jni.out");
	std::string actualOutput((std::istreambuf_iterator<char>(outputFile)), std::istreambuf_iterator<char>());
	outputFile.close();
	ASSERT_EQ(actualOutput, "ok\n") << "The actual output does not match the expected output.";
	fclose(file);
}