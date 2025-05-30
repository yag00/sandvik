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

#include <java.base/classes.hpp>
#include <classloader.hpp>
#include <object.hpp>

using namespace sandvik;


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
