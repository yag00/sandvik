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

TEST(VM, add) {
	logger.setLevel(Logger::LogLevel::NONE);
	Vm vm;

	// Redirect stdout to output.txt
	FILE* file = freopen("test_add.out", "w", stdout);
	ASSERT_NE(file, nullptr) << "Failed to redirect stdout";

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

}

TEST(VM, fibonacci) {
	logger.setLevel(Logger::LogLevel::NONE);
	Vm vm;

	// Redirect stdout to output.txt
	FILE* file = freopen("test_fibonacci.out", "w", stdout);
	ASSERT_NE(file, nullptr) << "Failed to redirect stdout";

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
}

TEST(VM, dalvik) {
	logger.setLevel(Logger::LogLevel::NONE);
	Vm vm;

	// Redirect stdout to output.txt
	FILE* file = freopen("test_dalvik.out", "w", stdout);
	ASSERT_NE(file, nullptr) << "Failed to redirect stdout";

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
}
