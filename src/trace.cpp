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

#include "trace.hpp"

#include <fmt/color.h>
#include <fmt/format.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

#include "disassembler.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

Trace::Trace() : _trace_instructions(false), _trace_calls(false), _disassembler(std::make_unique<Disassembler>()) {
}

void Trace::enableInstructionTrace(bool enable_) {
	_trace_instructions = enable_;
}
void Trace::enableCallTrace(bool enable_) {
	_trace_calls = enable_;
}

void Trace::logInstruction(const uint64_t pc_, const std::string& function_, const uint8_t* bytecode_) {
	if (!_trace_instructions) {
		return;
	}
	auto inst = _disassembler->disassemble(bytecode_);
	logger.log(Logger::LogLevel::INFO | Logger::LogLevel::ALWAYS, fmt::format("{:04x}: {:<80} {:<20} ", pc_ / 2, inst, function_));
}

void Trace::logCall(const std::string& type_, const std::string& class_, const std::string& method_, const std::string& signature_,
                    const std::vector<std::shared_ptr<Object>>& args_, bool static_) {
	if (!_trace_calls) {
		return;
	}
	std::string args_str = "(";
	for (size_t i = 0; i < args_.size(); ++i) {
		if (i == 0 && !static_) {
			args_str += "this=";
		}
		auto arg = args_[i];
		if (i > 0) {
			args_str += ", ";
		}
		args_str += (arg ? arg->toString() : std::string("null"));
	}
	args_str += ")";

	std::string msg = fmt::format("{} {}.{}{} {}", type_, fmt::styled(class_, fmt::fg(fmt::color::cyan)), fmt::styled(method_, fmt::fg(fmt::color::lawn_green)),
	                              fmt::styled(signature_, fmt::fg(fmt::color::yellow)), fmt::styled(args_str, fmt::fg(fmt::color::magenta)));
	logger.log(Logger::LogLevel::INFO | Logger::LogLevel::ALWAYS, msg);
}
