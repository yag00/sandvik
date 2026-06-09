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

TEST(VM, HelloWorld) {
	logger.setLevel(Logger::LogLevel::NONE);
	Vm vm;

	// Redirect stdout to output.txt
	FILE* file = freopen("test_helloworld.out", "w", stdout);
	ASSERT_NE(file, nullptr) << "Failed to redirect stdout";

	initializeVmRuntime(vm);
	vm.loadDex("../tests/java/hello/classes.dex");
	vm.run("HelloWorld", {});

	std::ifstream outputFile("test_helloworld.out");
	std::string actualOutput((std::istreambuf_iterator<char>(outputFile)),
								std::istreambuf_iterator<char>());
	outputFile.close();

	std::ifstream refFile("../tests/unit/test_helloworld.ref");
	std::string expectedOutput((std::istreambuf_iterator<char>(refFile)),
								std::istreambuf_iterator<char>());
	refFile.close();
	ASSERT_EQ(actualOutput, expectedOutput) << "The actual output does not match the expected output.";
	fclose(file);
}

TEST(VM, Fibonacci) {
	logger.setLevel(Logger::LogLevel::NONE);
	Vm vm;

	// Redirect stdout to output.txt
	FILE* file = freopen("test_fibonacci.out", "w", stdout);
	ASSERT_NE(file, nullptr) << "Failed to redirect stdout";

	initializeVmRuntime(vm);
	vm.loadDex("../tests/java/fib/classes.dex");
	vm.run("Fibonacci", {});

	std::ifstream outputFile("test_fibonacci.out");
	std::string actualOutput((std::istreambuf_iterator<char>(outputFile)),
							 std::istreambuf_iterator<char>());
	outputFile.close();

	std::ifstream refFile("../tests/unit/test_fibonacci.ref");
	std::string expectedOutput((std::istreambuf_iterator<char>(refFile)),
							   std::istreambuf_iterator<char>());
	refFile.close();
	ASSERT_EQ(actualOutput, expectedOutput) << "The actual output does not match the expected output.";
	fclose(file);
}

TEST(VM, Dalvik) {
	logger.setLevel(Logger::LogLevel::NONE);
	Vm vm;

	// Redirect stdout to output.txt
	FILE* file = freopen("test_dalvik.out", "w", stdout);
	ASSERT_NE(file, nullptr) << "Failed to redirect stdout";

	initializeVmRuntime(vm);
	vm.loadDex("../tests/java/dalvik/classes.dex");
	vm.run("DalvikTest", {});

	std::ifstream outputFile("test_dalvik.out");
	std::string actualOutput((std::istreambuf_iterator<char>(outputFile)),
							 std::istreambuf_iterator<char>());
	outputFile.close();

	std::ifstream refFile("../tests/unit/test_dalvik.ref");
	std::string expectedOutput((std::istreambuf_iterator<char>(refFile)),
							   std::istreambuf_iterator<char>());
	refFile.close();
	ASSERT_EQ(actualOutput, expectedOutput) << "The actual output does not match the expected output.";
	fclose(file);
}

TEST(VM, Native) {
	logger.setLevel(Logger::LogLevel::NONE);
	Vm vm;

	// Redirect stdout to output.txt
	FILE* file = freopen("test_native.out", "w", stdout);
	ASSERT_NE(file, nullptr) << "Failed to redirect stdout";

	initializeVmRuntime(vm);
	vm.loadDex("../tests/java/native/classes.dex");
	vm.run("Native", {});
	fflush(stdout);  // Ensure all output is flushed to the file

	std::ifstream outputFile("test_native.out");
	std::string actualOutput((std::istreambuf_iterator<char>(outputFile)),
							 std::istreambuf_iterator<char>());
	outputFile.close();

	std::ifstream refFile("../tests/unit/test_native.ref");
	std::string expectedOutput((std::istreambuf_iterator<char>(refFile)),
							   std::istreambuf_iterator<char>());
	refFile.close();
	ASSERT_EQ(actualOutput, expectedOutput) << "The actual output does not match the expected output.";
	fclose(file);
}
