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

using namespace sandvik;

TEST(VM, Add) {
	logger.setLevel(Logger::LogLevel::NONE);
	Vm vm;

	// Redirect stdout to output.txt
	FILE* file = freopen("test_add.out", "w", stdout);
	ASSERT_NE(file, nullptr) << "Failed to redirect stdout";

	vm.loadRt();
	vm.loadDex("../tests/java/add/classes.dex");
	vm.run("Add", {"5", "10"});

	std::ifstream outputFile("test_add.out");
	std::string actualOutput((std::istreambuf_iterator<char>(outputFile)),
								std::istreambuf_iterator<char>());
	outputFile.close();

	std::ifstream refFile("../tests/unit/test_add.ref");
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

	vm.loadRt();
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

	vm.loadRt("sanddirt.dex.jar");
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

	vm.loadRt();
	vm.loadDex("../tests/java/native/classes.dex");
	vm.run("Native", {});

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


void run_common_test(const std::string& mainclassname) {
	logger.setLevel(Logger::LogLevel::NONE);
	Vm vm;

	// Redirect stdout to output.txt
	FILE* file = freopen("test_DefaultMethods.out", "w", stdout);
	ASSERT_NE(file, nullptr) << "Failed to redirect stdout";

	vm.loadRt();
	vm.loadRt("../tests/java/unit/TestUnitDex.jar");
	vm.run(mainclassname, {});

	std::ifstream outputFile("test_DefaultMethods.out");
	std::string actualOutput((std::istreambuf_iterator<char>(outputFile)),
							 std::istreambuf_iterator<char>());
	outputFile.close();

	ASSERT_EQ(actualOutput, "ok\n") << "The actual output does not match the expected output.";

	fclose(file);
}

TEST(VM, DefaultMethods) {
	run_common_test("TestDefaultMethods");
}
TEST(VM, AbstractClass) {
	run_common_test("TestAbstractClass");
}
TEST(VM, MultiLevel) {
	run_common_test("TestMultiLevel");
}
TEST(VM, InterfaceConstants) {
	run_common_test("TestInterfaceConstants");
}
TEST(VM, InterfaceStatic) {
	run_common_test("TestInterfaceStatic");
}
TEST(VM, MethodConflict) {
	run_common_test("TestMethodConflict");
}
TEST(VM, Anonymous) {
	run_common_test("TestAnonymous");
}
TEST(VM, Arithmetic) {
	run_common_test("TestArithmetic");
}
TEST(VM, Arrays) {
	run_common_test("TestArrays");
}
TEST(VM, Exceptions) {
	run_common_test("TestExceptions");
}
TEST(VM, Loops) {
	run_common_test("TestLoops");
}
TEST(VM, Strings) {
	run_common_test("TestStrings");
}
TEST(VM, Statics) {
	run_common_test("TestStatics");
}
TEST(VM, DeepLoops) {
	run_common_test("TestDeepLoops");
}
TEST(VM, EdgeNumbers) {
	run_common_test("TestEdgeNumbers");
}
TEST(VM, Final) {
	run_common_test("TestFinal");
}

TEST(VM, SwitchCase) {
	run_common_test("TestSwitchCase");
}
TEST(VM, LargeSwitchDense) {
	run_common_test("TestLargeSwitchDense");
}
TEST(VM, LargeSwitchSparse) {
	run_common_test("TestLargeSwitchSparse");
}
TEST(VM, Polymorphism) {
	run_common_test("TestPolymorphism");
}

TEST(VM, Interfaces) {
	run_common_test("TestInterfaces");
}
TEST(VM, Diamond) {
	run_common_test("TestDiamond");
}
TEST(VM, InterfaceArray) {
	run_common_test("TestInterfaceArray");
}
TEST(VM, InstanceOf) {
	run_common_test("TestInstanceOf");
}
TEST(VM, CollectionsLike) {
	run_common_test("TestCollectionsLike");
}
TEST(VM, ExceptionsFlow) {
	run_common_test("TestExceptionsFlow");
}
TEST(VM, Recursion) {
	run_common_test("TestRecursion");
}
TEST(VM, StringOps) {
	run_common_test("TestStringOps");
}
TEST(VM, StaticInit) {
	run_common_test("TestStaticInit");
}
//TEST(VM, Enums) {
//	run_common_test("TestEnums");
//}
TEST(VM, Reflection) {
	run_common_test("TestReflection");
}
TEST(VM, InnerClasses) {
	run_common_test("TestInnerClasses");
}
TEST(VM, Autoboxing) {
	run_common_test("TestAutoboxing");
}
TEST(VM, Varargs) {
	run_common_test("TestVarargs");
}
TEST(VM, DeepStack) {
	run_common_test("TestDeepStack");
}
TEST(VM, SwitchOnString) {
	run_common_test("TestSwitchOnString");
}
