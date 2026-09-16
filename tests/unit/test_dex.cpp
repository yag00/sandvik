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

#include <algorithm>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "loader/dex/File.hpp"

using namespace sandvik;

namespace {
	std::unique_ptr<dex::File> loadDex(const std::string& path_) {
		std::ifstream file(path_, std::ios::binary);
		if (!file) {
			ADD_FAILURE() << "Could not open " << path_;
			return nullptr;
		}
		std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		return dex::File::parse(std::move(buffer));
	}

	const dex::Class* findClass(const dex::File& file_, const std::string& prettyName_) {
		for (const auto& cls : file_.classes()) {
			if (cls.pretty_name() == prettyName_) {
				return &cls;
			}
		}
		return nullptr;
	}

	const dex::Field* findField(const dex::Class& class_, const std::string& name_) {
		for (const auto& field : class_.fields()) {
			if (field.name() == name_) {
				return &field;
			}
		}
		return nullptr;
	}

	const dex::Method* findMethod(const dex::Class& class_, const std::string& name_) {
		for (const auto& method : class_.methods()) {
			if (method.name() == name_) {
				return &method;
			}
		}
		return nullptr;
	}

	const dex::Annotation* findAnnotation(const std::vector<dex::Annotation>& annotations_, const std::string& type_) {
		for (const auto& annotation : annotations_) {
			if (annotation.type() == type_) {
				return &annotation;
			}
		}
		return nullptr;
	}
}  // namespace

TEST(DexParser, LoadsBasicClassesDex) {
	auto file = loadDex("../tests/java/hello/classes.dex");
	ASSERT_NE(file, nullptr);

	const auto* helloWorld = findClass(*file, "HelloWorld");
	ASSERT_NE(helloWorld, nullptr);

	const auto* main = findMethod(*helloWorld, "main");
	ASSERT_NE(main, nullptr);
	EXPECT_TRUE(main->has(dex::ACC_STATIC));
	EXPECT_TRUE(main->has(dex::ACC_PUBLIC));
	EXPECT_TRUE(main->annotations().empty());
}

TEST(DexParser, FixtureHasExpectedClasses) {
	auto file = loadDex("../tests/java/annotations/classes.dex");
	ASSERT_NE(file, nullptr);

	EXPECT_NE(findClass(*file, "Fixture"), nullptr);
	EXPECT_NE(findClass(*file, "Fixture$Inner"), nullptr);
	EXPECT_NE(findClass(*file, "Fixture$Meta"), nullptr);
}

TEST(DexParser, EnclosingClassAnnotation) {
	auto file = loadDex("../tests/java/annotations/classes.dex");
	ASSERT_NE(file, nullptr);

	const auto* inner = findClass(*file, "Fixture$Inner");
	ASSERT_NE(inner, nullptr);

	const auto* enclosing = findAnnotation(inner->annotations(), "dalvik.annotation.EnclosingClass");
	ASSERT_NE(enclosing, nullptr);
	EXPECT_EQ(enclosing->visibility(), dex::AnnotationVisibility::SYSTEM);

	const auto* value = enclosing->element("value");
	ASSERT_NE(value, nullptr);
	EXPECT_EQ(value->kind(), dex::EncodedValue::Kind::Type);
	EXPECT_EQ(value->asString(), "Fixture");
}

TEST(DexParser, InnerClassAnnotationNameAndFlags) {
	auto file = loadDex("../tests/java/annotations/classes.dex");
	ASSERT_NE(file, nullptr);

	const auto* inner = findClass(*file, "Fixture$Inner");
	ASSERT_NE(inner, nullptr);

	const auto* innerClass = findAnnotation(inner->annotations(), "dalvik.annotation.InnerClass");
	ASSERT_NE(innerClass, nullptr);

	const auto* name = innerClass->element("name");
	ASSERT_NE(name, nullptr);
	EXPECT_EQ(name->kind(), dex::EncodedValue::Kind::String);
	EXPECT_EQ(name->asString(), "Inner");

	const auto* accessFlags = innerClass->element("accessFlags");
	ASSERT_NE(accessFlags, nullptr);
	EXPECT_EQ(accessFlags->kind(), dex::EncodedValue::Kind::Int);
	EXPECT_EQ(accessFlags->asInt(), static_cast<int64_t>(dex::ACC_STATIC));
}

TEST(DexParser, MemberClassesAnnotation) {
	auto file = loadDex("../tests/java/annotations/classes.dex");
	ASSERT_NE(file, nullptr);

	const auto* fixture = findClass(*file, "Fixture");
	ASSERT_NE(fixture, nullptr);

	const auto* memberClasses = findAnnotation(fixture->annotations(), "dalvik.annotation.MemberClasses");
	ASSERT_NE(memberClasses, nullptr);

	const auto* value = memberClasses->element("value");
	ASSERT_NE(value, nullptr);
	EXPECT_EQ(value->kind(), dex::EncodedValue::Kind::Array);

	const auto& items = value->asArray();
	ASSERT_EQ(items.size(), 2u);
	std::vector<std::string> names;
	for (const auto& item : items) {
		EXPECT_EQ(item.kind(), dex::EncodedValue::Kind::Type);
		names.push_back(item.asString());
	}
	EXPECT_NE(std::find(names.begin(), names.end(), "Fixture$Inner"), names.end());
	EXPECT_NE(std::find(names.begin(), names.end(), "Fixture$Meta"), names.end());
}

TEST(DexParser, SignatureAnnotationOnField) {
	auto file = loadDex("../tests/java/annotations/classes.dex");
	ASSERT_NE(file, nullptr);

	const auto* fixture = findClass(*file, "Fixture");
	ASSERT_NE(fixture, nullptr);

	const auto* field = findField(*fixture, "genericField");
	ASSERT_NE(field, nullptr);

	const auto* signature = findAnnotation(field->annotations(), "dalvik.annotation.Signature");
	ASSERT_NE(signature, nullptr);

	const auto* value = signature->element("value");
	ASSERT_NE(value, nullptr);
	EXPECT_EQ(value->kind(), dex::EncodedValue::Kind::Array);

	std::string signatureStr;
	for (const auto& item : value->asArray()) {
		EXPECT_EQ(item.kind(), dex::EncodedValue::Kind::String);
		signatureStr += item.asString();
	}
	EXPECT_EQ(signatureStr, "Ljava/util/List<Ljava/lang/String;>;");
}

TEST(DexParser, CustomAnnotationOnMethod) {
	auto file = loadDex("../tests/java/annotations/classes.dex");
	ASSERT_NE(file, nullptr);

	const auto* fixture = findClass(*file, "Fixture");
	ASSERT_NE(fixture, nullptr);

	const auto* annotated = findMethod(*fixture, "annotated");
	ASSERT_NE(annotated, nullptr);

	const auto* meta = findAnnotation(annotated->annotations(), "Fixture$Meta");
	ASSERT_NE(meta, nullptr);
	EXPECT_EQ(meta->visibility(), dex::AnnotationVisibility::RUNTIME);

	const auto* intVal = meta->element("intVal");
	ASSERT_NE(intVal, nullptr);
	EXPECT_EQ(intVal->kind(), dex::EncodedValue::Kind::Int);
	EXPECT_EQ(intVal->asInt(), 7);

	const auto* strVal = meta->element("strVal");
	ASSERT_NE(strVal, nullptr);
	EXPECT_EQ(strVal->kind(), dex::EncodedValue::Kind::String);
	EXPECT_EQ(strVal->asString(), "hi");

	const auto* boolVal = meta->element("boolVal");
	ASSERT_NE(boolVal, nullptr);
	EXPECT_EQ(boolVal->kind(), dex::EncodedValue::Kind::Bool);
	EXPECT_TRUE(boolVal->asBool());

	const auto* arrVal = meta->element("arrVal");
	ASSERT_NE(arrVal, nullptr);
	EXPECT_EQ(arrVal->kind(), dex::EncodedValue::Kind::Array);
	ASSERT_EQ(arrVal->asArray().size(), 2u);
	EXPECT_EQ(arrVal->asArray()[0].asString(), "a");
	EXPECT_EQ(arrVal->asArray()[1].asString(), "b");

	EXPECT_EQ(meta->element("noSuchElement"), nullptr);
}

TEST(DexParser, CustomAnnotationOnParameter) {
	auto file = loadDex("../tests/java/annotations/classes.dex");
	ASSERT_NE(file, nullptr);

	const auto* fixture = findClass(*file, "Fixture");
	ASSERT_NE(fixture, nullptr);

	const auto* annotated = findMethod(*fixture, "annotated");
	ASSERT_NE(annotated, nullptr);

	const auto& paramAnnotations = annotated->parameterAnnotations(0);
	ASSERT_EQ(paramAnnotations.size(), 1u);

	const auto& meta = paramAnnotations[0];
	EXPECT_EQ(meta.type(), "Fixture$Meta");

	const auto* intVal = meta.element("intVal");
	ASSERT_NE(intVal, nullptr);
	EXPECT_EQ(intVal->asInt(), 1);

	const auto* strVal = meta.element("strVal");
	ASSERT_NE(strVal, nullptr);
	EXPECT_EQ(strVal->asString(), "p");

	const auto* boolVal = meta.element("boolVal");
	ASSERT_NE(boolVal, nullptr);
	EXPECT_FALSE(boolVal->asBool());

	const auto* arrVal = meta.element("arrVal");
	ASSERT_NE(arrVal, nullptr);
	EXPECT_EQ(arrVal->kind(), dex::EncodedValue::Kind::Array);
	EXPECT_TRUE(arrVal->asArray().empty());

	// Out-of-range parameter index falls back to an empty set rather than throwing.
	EXPECT_TRUE(annotated->parameterAnnotations(5).empty());
}
