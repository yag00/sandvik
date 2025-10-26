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

#include <thread>
#include <chrono>
#include <string.h>
#include <gtest/gtest.h>

#include <class.hpp>
#include <classbuilder.hpp>
#include <classloader.hpp>
#include <field.hpp>
#include <frame.hpp>
#include <array.hpp>
#include <method.hpp>
#include <monitor.hpp>
#include <object.hpp>
#include <system/logger.hpp>

using namespace sandvik;

namespace {
	class String {
		public:
			static void equals(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				auto str0 = args_[0];
				auto str1 = args_[1];
				if (str0->isString() && str1->isString()) {
					if (str0->str() == str1->str()) {
						frame_.setReturnValue(1);
					}
				}
				frame_.setReturnValue(0);
			}
			static void length(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 1) {
					throw std::runtime_error("Not enough arguments");
				}
				auto strobj = args_[0];
				if (!strobj->isString()) {
					throw std::runtime_error("First argument is not an instance of java.lang.String");
				}
				frame_.setReturnValue(strobj->str().length());
			}
	};
}  // namespace

namespace java {
	namespace lang {
		void String(::sandvik::ClassLoader& classLoader) {
			ClassBuilder builder(classLoader, "java.lang", "java.lang.String");
			builder.setSuperClass("java.lang.Object");
			builder.addVirtualMethod("equals", "(Ljava/lang/Object;)Z", 0, String::equals);
			builder.addVirtualMethod("length", "()I", 0, String::length);
			builder.finalize();
		}
	}  // namespace lang
}  // namespace java


TEST(object, number) {
	ClassLoader classloader;
	java::lang::String(classloader);
	auto obj_null = Object::makeNull();
	auto obj_zero = Object::make(0);
	auto obj_a = Object::make(1234);
	auto obj_b = Object::make(1234);
	auto obj_c = Object::make(5678);
	auto obj_s = Object::make(classloader, "1234");

	auto& ref_null = *obj_null.get();
	auto& ref_zero = *obj_zero.get();
	auto& ref_a = *obj_a.get();
	auto& ref_b = *obj_b.get();
	auto& ref_c = *obj_c.get();
	auto& ref_s = *obj_s.get();

	EXPECT_TRUE(ref_a == ref_b);
	EXPECT_TRUE(ref_b == ref_a);
	EXPECT_FALSE(ref_a != ref_b);
	EXPECT_FALSE(ref_b != ref_a);

	EXPECT_FALSE(ref_a == ref_null);
	EXPECT_FALSE(ref_a == ref_zero);
	EXPECT_FALSE(ref_a == ref_c);
	EXPECT_FALSE(ref_c == ref_a);
	EXPECT_FALSE(ref_b == ref_c);
	EXPECT_FALSE(ref_c == ref_b);

	EXPECT_TRUE(ref_a != ref_null);
	EXPECT_TRUE(ref_a != ref_zero);
	EXPECT_TRUE(ref_a != ref_c);
	EXPECT_TRUE(ref_c != ref_a);
	EXPECT_TRUE(ref_b != ref_c);
	EXPECT_TRUE(ref_c != ref_b);

	EXPECT_FALSE(ref_a == ref_s);
	EXPECT_TRUE(ref_a != ref_s);
}

TEST(object, string) {
	ClassLoader classloader;
	java::lang::String(classloader);
	auto obj_null = Object::makeNull();
	auto obj_a = Object::make(classloader, "Hello");
	auto obj_b = Object::make(classloader, "Hello");
	auto obj_c = Object::make(classloader, "Hello2");
	auto obj_i = Object::make(1);

	auto& ref_null = *obj_null.get();
	auto& ref_a = *obj_a.get();
	auto& ref_b = *obj_b.get();
	auto& ref_c = *obj_c.get();
	auto& ref_i = *obj_i.get();

	EXPECT_FALSE(ref_a == ref_null);
	EXPECT_TRUE(ref_a != ref_null);
	EXPECT_FALSE(ref_a == ref_i);
	EXPECT_TRUE(ref_a != ref_i);

	EXPECT_TRUE(ref_a == ref_b);
	EXPECT_TRUE(ref_b == ref_a);
	EXPECT_FALSE(ref_a != ref_b);
	EXPECT_FALSE(ref_b != ref_a);

	EXPECT_FALSE(ref_a == ref_c);
	EXPECT_FALSE(ref_c == ref_a);
	EXPECT_TRUE(ref_b != ref_c);
	EXPECT_TRUE(ref_c != ref_b);
}

TEST(object, array) {
	ClassLoader classloader;
	ClassBuilder(classloader, "", "int").finalize();

	auto obj_null = Object::makeNull();

	auto obj = Object::makeArray(classloader, classloader.getOrLoad("int"), {3, 3});
	auto array = std::dynamic_pointer_cast<Array>(obj);
	logger.fdebug("{}", array->debug());

	for (uint32_t i = 0; i < 3; ++i) {
		for (uint32_t j = 0; j < 3; ++j) {
			auto elem = array->getElement({i, j});
			logger.fdebug("Element [{}][{}]: {}", i, j, elem->debug());
			EXPECT_TRUE(elem->isNull());
			array->setElement({i, j}, Object::make(i * 3 + j + 1));
		}
	}
	for (uint32_t i = 0; i < 3; ++i) {
		auto subarray = array->getArray(i);
		logger.fdebug("Sub-array [{}]: {}", i, subarray->debug());
		for (uint32_t j = 0; j < 3; ++j) {
			auto elem = subarray->getElement(j);
			logger.fdebug("Element [{}][{}]: {}", i, j, elem->debug());
			ASSERT_NE(elem, nullptr);
			EXPECT_EQ(elem->getValue(), static_cast<int32_t>(i * 3 + j + 1));
			subarray->setElement(j, Object::make(elem->getValue() + 0x10));
		}
	}
	for (uint32_t i = 0; i < 3; ++i) {
		for (uint32_t j = 0; j < 3; ++j) {
			auto elem = array->getElement({i, j});
			logger.fdebug("Element [{}][{}]: {}", i, j, elem->debug());
			ASSERT_NE(elem, nullptr);
			EXPECT_EQ(elem->getValue(), static_cast<int32_t>(i * 3 + j + 0x11));
		}
	}
}

TEST(object, lock) {
	logger.setLevel(Logger::LogLevel::DEBUG);
	auto obj = std::make_shared<Object>();
	obj->setField("value", Object::make(42));
	obj->monitorEnter();

	std::thread writer([obj]() {
		auto val = obj->getField("value");
		obj->setField("value", Object::make(43));
	});
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	EXPECT_EQ(obj->getField("value")->getValue(), 42);
	std::this_thread::yield();
	obj->setField("value", Object::make(2));
	EXPECT_EQ(obj->getField("value")->getValue(), 2);
	obj->monitorExit();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	EXPECT_EQ(obj->getField("value")->getValue(), 43);

	writer.join();
}
