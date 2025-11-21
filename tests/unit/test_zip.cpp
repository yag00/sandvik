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

#include <system/zip.hpp>

using namespace sandvik;


TEST(compression, zip) {
	std::string data = "Hello World";
	std::string data2 = "Good Bye!";
	ZipWriter zipw;
	zipw.open("ziptest.zip");
	zipw.addFromMemory("test.txt", data.c_str(), data.size());
	zipw.addFromMemory("test2.txt", data2.c_str(), data2.size());
	zipw.close();

	ZipReader zipr;
	zipr.open("ziptest.zip");

	uint64_t size = 0;
	auto ptr = zipr.extractToMemory("test.txt", size);
	std::string rdata(ptr.get(), size);

	ptr = zipr.extractToMemory("test2.txt", size);
	std::string rdata2(ptr.get(), size);

	zipr.close();

	EXPECT_EQ(data, rdata);
	EXPECT_EQ(data2, rdata2);
}
