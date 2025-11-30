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
#include <monitor.hpp>
#include <object.hpp>
#include <system/logger.hpp>
#include <gc.hpp>

using namespace sandvik;

TEST(gc, collect) {
	GC::getInstance().run();
	EXPECT_EQ(GC::getInstance().getTrackedObjectCount(), 0u);
	for (uint32_t i = 0; i < 50; ++i) {
		Object::make(i);
	}
	EXPECT_EQ(GC::getInstance().getTrackedObjectCount(), 50u);
	EXPECT_EQ(GC::getInstance().getGcCycles(), 0u);
	GC::getInstance().requestCollect();
	uint32_t cpt = 20;
	while (GC::getInstance().getGcCycles() == 0u && cpt > 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		--cpt;
	}
	EXPECT_EQ(GC::getInstance().getGcCycles(), 1u);
	EXPECT_EQ(GC::getInstance().getTrackedObjectCount(), 0u);
	GC::getInstance().stop();
}
