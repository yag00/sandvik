#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <vm.hpp>
#include <classloader.hpp>
#include <jni.hpp>
#include <system/logger.hpp>
#include "common.hpp"

using namespace sandvik;

class JNITestFixture : public ::testing::Test {
protected:
    Vm vm;

    void SetUp() override {
        logger.setLevel(Logger::LogLevel::NONE);
        initializeVmRuntime(vm);
        vm.loadRt();
        vm.loadDex("../tests/java/jni/classes.dex");
    }

    void runJNITestClass(const std::string &className, const std::string &outFile) {
        FILE* file = freopen(outFile.c_str(), "w", stdout);
        ASSERT_NE(file, nullptr) << "Failed to redirect stdout for " << className;

        vm.run(className, {});

        fflush(stdout);
        fclose(file);

        // Verify Output
        std::ifstream outputFile(outFile);
        std::string actualOutput((std::istreambuf_iterator<char>(outputFile)), std::istreambuf_iterator<char>());
        outputFile.close();

        EXPECT_EQ(actualOutput, "ok\n") << "Test failed for class: " << className;
    }
};

TEST_F(JNITestFixture, NativeHelloWorld) {
    runJNITestClass("TestJNIHelloWorld", "test_jni_helloworld.out");
}

TEST_F(JNITestFixture, NativeStrings) {
    runJNITestClass("TestJNIStrings", "test_jni_strings.out");
}

TEST_F(JNITestFixture, NativeTypes) {
    runJNITestClass("TestJNITypes", "test_jni_types.out");
}

TEST_F(JNITestFixture, NativeFields) {
    runJNITestClass("TestJNIFields", "test_jni_fields.out");
}

TEST_F(JNITestFixture, NativeMethods) {
    runJNITestClass("TestJNIMethods", "test_jni_methods.out");
}

TEST_F(JNITestFixture, NativeObjects) {
    runJNITestClass("TestJNIObjects", "test_jni_objects.out");
}

TEST_F(JNITestFixture, NativeArrays) {
    runJNITestClass("TestJNIArrays", "test_jni_arrays.out");
}

TEST_F(JNITestFixture, NativeExceptions) {
    runJNITestClass("TestJNIExceptions", "test_jni_exceptions.out");
}

TEST_F(JNITestFixture, NativeReferences) {
    runJNITestClass("TestJNIRefs", "test_jni_refs.out");
}

TEST_F(JNITestFixture, NativeThreads) {
    runJNITestClass("TestJNIThreads", "test_jni_threads.out");
}