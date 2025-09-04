#include <string.h>
#include <unistd.h>

#include <fmt/format.h>
#include <jni/jni.h>

#include <iostream>
#include <stdexcept>

#include "object.hpp"
#include "system/logger.hpp"

extern "C" {
	JNIEXPORT void JNICALL Java_java_io_PrintStream_print__Ljava_lang_String_2(JNIEnv* env, jobject obj, jstring str) {
		sandvik::Object* this_ptr = (sandvik::Object*)obj;
		if (this_ptr == nullptr) {
			throw std::runtime_error("NullPointerException");
		}
		auto fileObj = this_ptr->getField("file");
		int fd = fileObj->getValue();
		const char* nativeStr = env->GetStringUTFChars(str, nullptr);
		if (nativeStr != nullptr) {
			switch (fd) {
				case 1:
					std::cout << nativeStr;
					break;
				case 2:
					std::cerr << nativeStr;
					break;
				default:
					ssize_t written = write(fd, nativeStr, strlen(nativeStr));
					if (written == -1) {
						logger.fwarning("Failed to write {} to file descriptor {}", nativeStr, fd);
					}
					break;
			}
			env->ReleaseStringUTFChars(str, nativeStr);
		}
	}
	JNIEXPORT void JNICALL Java_java_io_PrintStream_println__Ljava_lang_String_2(JNIEnv* env, jobject obj, jstring str) {
		sandvik::Object* this_ptr = (sandvik::Object*)obj;
		if (this_ptr == nullptr) {
			throw std::runtime_error("NullPointerException");
		}
		auto fileObj = this_ptr->getField("file");
		int fd = fileObj->getValue();
		const char* nativeStr = env->GetStringUTFChars(str, nullptr);
		if (nativeStr != nullptr) {
			switch (fd) {
				case 1:
					std::cout << nativeStr << std::endl;
					break;
				case 2:
					std::cerr << nativeStr << std::endl;
					break;
				default:
					ssize_t written = write(fd, nativeStr, strlen(nativeStr));
					if (written == -1) {
						logger.fwarning("Failed to write {} to file descriptor {}", nativeStr, fd);
					}
					break;
			}
			env->ReleaseStringUTFChars(str, nativeStr);
		}
	}
	JNIEXPORT void JNICALL Java_java_io_PrintStream_println__(JNIEnv* env, jobject obj) {
		sandvik::Object* this_ptr = (sandvik::Object*)obj;
		if (this_ptr == nullptr) {
			throw std::runtime_error("NullPointerException");
		}
		auto fileObj = this_ptr->getField("file");
		int fd = fileObj->getValue();
		switch (fd) {
			case 1:
				std::cout << std::endl;
				break;
			case 2:
				std::cerr << std::endl;
				break;
			default:
				const char* newline = "\n";
				ssize_t written = write(fd, newline, strlen(newline));
				if (written == -1) {
					logger.fwarning("Failed to write newline to file descriptor {}", fd);
				}
				break;
		}
	}
}
