#include <string.h>
#include <gtest/gtest.h>

#include <object.hpp>

using namespace sandvik;


TEST(object, number) {
	auto obj_null = Object::makeNull();
	auto obj_zero = Object::make(0);
	auto obj_a = Object::make(1234);
	auto obj_b = Object::make(1234);
	auto obj_c = Object::make(5678);
	auto obj_s = Object::make("1234");

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
	auto obj_null = Object::makeNull();
	auto obj_a = Object::make("Hello");
	auto obj_b = Object::make("Hello");
	auto obj_c = Object::make("Hello2");
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
