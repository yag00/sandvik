#include <string.h>
#include <unistd.h>

#include <fmt/format.h>
#include <jni/jni.h>

#include <iostream>
#include <stdexcept>

#include "exceptions.hpp"
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
		sandvik::Object* strobj = (sandvik::Object*)str;
		if (strobj == nullptr) {
			throw sandvik::NullPointerException("Null String object");
		}
		if (!strobj->isString()) {
			throw sandvik::ClassCastException("Not a string");
		}
		const auto s = strobj->str();
		switch (fd) {
			case 1:
				std::cout << s;
				break;
			case 2:
				std::cerr << s;
				break;
			default:
				ssize_t written = write(fd, s.c_str(), s.size());
				if (written == -1) {
					logger.fwarning("Failed to write {} to file descriptor {}", s, fd);
				}
				break;
		}
	}
	JNIEXPORT void JNICALL Java_java_io_PrintStream_println__Ljava_lang_String_2(JNIEnv* env, jobject obj, jstring str) {
		sandvik::Object* this_ptr = (sandvik::Object*)obj;
		if (this_ptr == nullptr) {
			throw std::runtime_error("NullPointerException");
		}
		auto fileObj = this_ptr->getField("file");
		int fd = fileObj->getValue();
		sandvik::Object* strobj = (sandvik::Object*)str;
		if (strobj == nullptr) {
			throw sandvik::NullPointerException("Null String object");
		}
		if (!strobj->isString()) {
			throw sandvik::ClassCastException("Not a string");
		}
		const auto s = strobj->str();
		switch (fd) {
			case 1:
				std::cout << s << std::endl;
				break;
			case 2:
				std::cerr << s << std::endl;
				break;
			default:
				ssize_t written = write(fd, s.c_str(), s.size());
				if (written == -1) {
					logger.fwarning("Failed to write {} to file descriptor {}", s, fd);
				}
				break;
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
				ssize_t written = write(fd, "\n", 1);
				if (written == -1) {
					logger.fwarning("Failed to write newline to file descriptor {}", fd);
				}
				break;
		}
	}
}
