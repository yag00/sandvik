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

void run_common_test(const std::string& mainclassname) {
	logger.setLevel(Logger::LogLevel::NONE);
	Vm vm;

	// Redirect stdout to output.txt
	auto filename = fmt::format("test_{}.out", mainclassname);
	FILE* file = freopen(filename.c_str(), "w", stdout);
	ASSERT_NE(file, nullptr) << "Failed to redirect stdout";

	try {
		initializeVmRuntime(vm);
		vm.loadRt("../tests/java/unit/TestUnitDex.jar");
		vm.run(mainclassname, {});
	} catch (const std::exception& e) {
		FAIL() << fmt::format("Exception thrown during test {}: {}", mainclassname, e.what());
	}
	std::ifstream outputFile(filename.c_str());
	std::string actualOutput((std::istreambuf_iterator<char>(outputFile)),
							 std::istreambuf_iterator<char>());
	outputFile.close();

	ASSERT_EQ(actualOutput, "ok\n") << "The actual output does not match the expected output.";

	fclose(file);
}

TEST(JVM, DefaultMethods) {
	run_common_test("TestDefaultMethods");
}
TEST(JVM, AbstractClass) {
	run_common_test("TestAbstractClass");
}
TEST(JVM, MultiLevel) {
	run_common_test("TestMultiLevel");
}
TEST(JVM, InterfaceConstants) {
	run_common_test("TestInterfaceConstants");
}
TEST(JVM, InterfaceStatic) {
	run_common_test("TestInterfaceStatic");
}
TEST(JVM, MethodConflict) {
	run_common_test("TestMethodConflict");
}
TEST(JVM, Anonymous) {
	run_common_test("TestAnonymous");
}
TEST(JVM, Arithmetic) {
	run_common_test("TestArithmetic");
}
TEST(JVM, Arrays) {
	run_common_test("TestArrays");
}
TEST(JVM, Exceptions) {
	run_common_test("TestExceptions");
}
TEST(JVM, Loops) {
	run_common_test("TestLoops");
}
TEST(JVM, Strings) {
	run_common_test("TestStrings");
}
TEST(JVM, Statics) {
	run_common_test("TestStatics");
}
TEST(JVM, DeepLoops) {
	run_common_test("TestDeepLoops");
}
TEST(JVM, EdgeNumbers) {
	run_common_test("TestEdgeNumbers");
}
TEST(JVM, Final) {
	run_common_test("TestFinal");
}

TEST(JVM, SwitchCase) {
	run_common_test("TestSwitchCase");
}
TEST(JVM, LargeSwitchDense) {
	run_common_test("TestLargeSwitchDense");
}
TEST(JVM, LargeSwitchSparse) {
	run_common_test("TestLargeSwitchSparse");
}
TEST(JVM, Polymorphism) {
	run_common_test("TestPolymorphism");
}

TEST(JVM, Interfaces) {
	run_common_test("TestInterfaces");
}
TEST(JVM, Diamond) {
	run_common_test("TestDiamond");
}
TEST(JVM, InterfaceArray) {
	run_common_test("TestInterfaceArray");
}
TEST(JVM, InstanceOf) {
	run_common_test("TestInstanceOf");
}
TEST(JVM, CollectionsLike) {
	run_common_test("TestCollectionsLike");
}
TEST(JVM, ExceptionsFlow) {
	run_common_test("TestExceptionsFlow");
}
TEST(JVM, TestExceptionsNoRehandle) {
	run_common_test("TestExceptionsNoRehandle");
}
TEST(JVM, Recursion) {
	run_common_test("TestRecursion");
}
TEST(JVM, StringOps) {
	run_common_test("TestStringOps");
}
TEST(JVM, StaticInit) {
	run_common_test("TestStaticInit");
}
TEST(JVM, Enums) {
	run_common_test("TestEnums");
}
//TEST(JVM, Reflection) {
//	run_common_test("TestReflection");
//}
TEST(JVM, InnerClasses) {
	run_common_test("TestInnerClasses");
}
TEST(JVM, Autoboxing) {
	run_common_test("TestAutoboxing");
}
TEST(JVM, Varargs) {
	run_common_test("TestVarargs");
}
TEST(JVM, DeepStack) {
	run_common_test("TestDeepStack");
}
TEST(JVM, SwitchOnString) {
	run_common_test("TestSwitchOnString");
}
TEST(JVM, Thread) {
	run_common_test("TestThread");
}
TEST(JVM, Thread2) {
	run_common_test("TestThread2");
}
TEST(JVM, ThreadComplex) {
	run_common_test("TestThreadComplex");
}
TEST(JVM, InvokeSuper) {
	run_common_test("TestInvokeSuper");
}
TEST(JVM, AtomicInteger) {
	run_common_test("TestAtomicInteger");
}
TEST(JVM, AtomicLong) {
	run_common_test("TestAtomicLong");
}
//TEST(JVM, Regex) {
//	disable for now, as icu classes are not implemented. Need to get the android icu jar or make a stub...
//	run_common_test("TestRegex");
//}
TEST(JVM, InvokeVirtualRange) {
	run_common_test("TestInvokeVirtualRange");
}
TEST(JVM, InvokeInterfaceRange) {
	run_common_test("TestInvokeInterfaceRange");
}
TEST(JVM, FilledNewArray) {
	run_common_test("TestFilledNewArray");
}
TEST(JVM, MonitorReentrant) {
	run_common_test("TestMonitorReentrant");
}
TEST(JVM, MonitorException) {
	run_common_test("TestMonitorException");
}
