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

#include <class.hpp>
#include <classbuilder.hpp>
#include <classloader.hpp>
#include <field.hpp>
#include <frame.hpp>
#include <method.hpp>
#include <object.hpp>

using namespace sandvik;

namespace {
	class String {
		public:
			static void equals(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				auto str0 = std::dynamic_pointer_cast<StringObject>(args_[0]);
				auto str1 = std::dynamic_pointer_cast<StringObject>(args_[1]);
				if (str0 != nullptr && str1 != nullptr) {
					if (str0->str() == str1->str()) {
						frame_.setReturnValue(1);
					} else {
						frame_.setReturnValue(0);
					}
				} else {
					throw std::runtime_error("First or second argument is not an instance of java.lang.String");
				}
			}
			static void length(Frame& frame_, std::vector<std::shared_ptr<Object>>& args_) {
				if (args_.size() < 1) {
					throw std::runtime_error("Not enough arguments");
				}
				auto clazz = std::dynamic_pointer_cast<StringObject>(args_[0]);
				if (!clazz) {
					throw std::runtime_error("First argument is not an instance of java.lang.String");
				}
				frame_.setReturnValue(clazz->str().length());
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
