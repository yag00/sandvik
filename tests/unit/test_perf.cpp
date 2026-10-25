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

#include <unistd.h>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <gtest/gtest.h>
#include <fmt/core.h>

#include <vm.hpp>
#include <system/logger.hpp>
#include "common.hpp"

using namespace sandvik;

/*
 * Performance regression reference for the VM.
 *
 * Each workload of tests/java/perf/Bench.java runs in a fresh VM. The checksum must match exactly (correctness)
 * and the wall time of Vm::run must stay under `refMs * factor`. refMs was measured on the reference machine
 * with a -O3 build; wall times are machine dependent, so the tolerance is configurable:
 *   SANDVIK_PERF_FACTOR=3.0   scale of the allowed time (default 2.0)
 *   SANDVIK_PERF_SKIP=1       only check checksums, ignore the timings
 */
namespace {
	struct Workload {
		const char* name;
		const char* checksum;
		double refMs;
	};

	const std::vector<Workload> workloads = {
		{"fib", "46368", 250},
		{"loop", "985951", 1450},
		{"arrays", "39780100000", 700},
		{"fields", "2550000", 950},
		{"virtual", "3100000", 830},
		{"alloc", "4999950000", 3970},
		{"strings", "267500", 165},
		{"collections", "16495500", 700},
	};

	double envDouble(const char* name_, double default_) {
		const char* v = std::getenv(name_);
		return v ? std::atof(v) : default_;
	}
}  // namespace

class Perf : public ::testing::TestWithParam<Workload> {};

TEST_P(Perf, Workload) {
	const auto& w = GetParam();
	logger.setLevel(Logger::LogLevel::NONE);

	auto filename = fmt::format("test_perf_{}.out", w.name);
	fflush(stdout);
	int savedStdout = dup(STDOUT_FILENO);
	ASSERT_GE(savedStdout, 0);
	ASSERT_NE(freopen(filename.c_str(), "w", stdout), nullptr) << "Failed to redirect stdout";

	double elapsedMs = 0;
	try {
		Vm vm;
		initializeVmRuntime(vm);
		vm.loadDex("../tests/java/perf/classes.dex");
		auto start = std::chrono::steady_clock::now();
		vm.run("Bench", {w.name});
		elapsedMs = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
	} catch (const std::exception& e) {
		fflush(stdout);
		dup2(savedStdout, STDOUT_FILENO);
		close(savedStdout);
		FAIL() << fmt::format("Exception thrown during perf workload {}: {}", w.name, e.what());
	}
	fflush(stdout);
	dup2(savedStdout, STDOUT_FILENO);
	close(savedStdout);

	std::ifstream outputFile(filename);
	std::string actual((std::istreambuf_iterator<char>(outputFile)), std::istreambuf_iterator<char>());
	ASSERT_EQ(actual, fmt::format("{} {}\n", w.name, w.checksum));

	fmt::print( "[perf] {:<12} {:>9.1f} ms (ref {:.1f} ms)\n", w.name, elapsedMs, w.refMs);
	if (envDouble("SANDVIK_PERF_SKIP", 0) == 0 && w.refMs > 0) {
		double limit = w.refMs * envDouble("SANDVIK_PERF_FACTOR", 2.0);
		EXPECT_LE(elapsedMs, limit) << fmt::format("perf regression on '{}': {:.1f} ms > {:.1f} ms allowed (ref {:.1f} ms)",
												   w.name, elapsedMs, limit, w.refMs);
	}
}

INSTANTIATE_TEST_SUITE_P(Vm, Perf, ::testing::ValuesIn(workloads), [](const auto& info) { return std::string(info.param.name); });
