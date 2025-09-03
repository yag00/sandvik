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
#include <array.hpp>
#include <object.hpp>
#include <system/logger.hpp>

using namespace sandvik;

TEST(array, multi_dimensions) {
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
			auto num_elem = std::dynamic_pointer_cast<NumberObject>(elem);
			ASSERT_NE(num_elem, nullptr);
			EXPECT_EQ(num_elem->getValue(), static_cast<int32_t>(i * 3 + j + 1));
			subarray->setElement(j, Object::make(num_elem->getValue() + 0x10));
		}
	}
	for (uint32_t i = 0; i < 3; ++i) {
		for (uint32_t j = 0; j < 3; ++j) {
			auto elem = array->getElement({i, j});
			logger.fdebug("Element [{}][{}]: {}", i, j, elem->debug());
			auto num_elem = std::dynamic_pointer_cast<NumberObject>(elem);
			ASSERT_NE(num_elem, nullptr);
			EXPECT_EQ(num_elem->getValue(), static_cast<int32_t>(i * 3 + j + 0x11));
		}
	}
}
