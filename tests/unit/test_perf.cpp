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
 * and the best of 3 wall times of Vm::run must stay under `refMs * factor + 250 ms`. refMs was measured on the reference machine
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

// Returns the wall time of one fresh-VM run, or a negative value when the output checksum does not match.
static double runOnce(const Workload& w_, std::string& error_) {
	auto filename = fmt::format("test_perf_{}.out", w_.name);
	fflush(stdout);
	int savedStdout = dup(STDOUT_FILENO);
	if (savedStdout < 0 || freopen(filename.c_str(), "w", stdout) == nullptr) {
		error_ = "Failed to redirect stdout";
		return -1;
	}

	double elapsedMs = -1;
	try {
		Vm vm;
		initializeVmRuntime(vm);
		vm.loadDex("../tests/java/perf/classes.dex");
		auto start = std::chrono::steady_clock::now();
		vm.run("Bench", {w_.name});
		elapsedMs = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
	} catch (const std::exception& e) {
		error_ = fmt::format("Exception thrown during perf workload {}: {}", w_.name, e.what());
	}
	fflush(stdout);
	dup2(savedStdout, STDOUT_FILENO);
	close(savedStdout);
	if (elapsedMs < 0) {
		return -1;
	}

	std::ifstream outputFile(filename);
	std::string actual((std::istreambuf_iterator<char>(outputFile)), std::istreambuf_iterator<char>());
	if (actual != fmt::format("{} {}\n", w_.name, w_.checksum)) {
		error_ = fmt::format("Unexpected output for {}: '{}'", w_.name, actual);
		return -1;
	}
	return elapsedMs;
}

TEST_P(Perf, Workload) {
	const auto& w = GetParam();
	logger.setLevel(Logger::LogLevel::NONE);

	const bool check = envDouble("SANDVIK_PERF_SKIP", 0) == 0 && w.refMs > 0;
	// The relative factor is meaningless for short workloads under load, so keep an absolute slack as well.
	const double limit = w.refMs * envDouble("SANDVIK_PERF_FACTOR", 2.0) + 250;
	const int maxAttempts = 3;

	// Best of N: a single slow run (parallel test binaries, cold caches) must not fail the test.
	double best = -1;
	for (int i = 0; i < maxAttempts; i++) {
		std::string error;
		double ms = runOnce(w, error);
		ASSERT_GE(ms, 0) << error;
		best = (best < 0 || ms < best) ? ms : best;
		if (!check || best <= limit) {
			break;
		}
	}

	fmt::print("[perf] {:<12} {:>9.1f} ms (ref {:.1f} ms)\n", w.name, best, w.refMs);
	if (check) {
		EXPECT_LE(best, limit) << fmt::format("perf regression on '{}': {:.1f} ms > {:.1f} ms allowed (ref {:.1f} ms)", w.name,
											  best, limit, w.refMs);
	}
}

INSTANTIATE_TEST_SUITE_P(Vm, Perf, ::testing::ValuesIn(workloads), [](const auto& info) { return std::string(info.param.name); });
