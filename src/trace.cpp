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

#include <bit>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>

#include "disassembler.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

namespace {
	std::vector<std::string> parseParameterDescriptors(const std::string& signature) {
		std::vector<std::string> params;
		auto start = signature.find('(');
		auto end = signature.find(')', start == std::string::npos ? 0 : start + 1);
		if (start == std::string::npos || end == std::string::npos || end <= start + 1) {
			return params;
		}

		for (size_t i = start + 1; i < end;) {
			size_t paramStart = i;
			while (i < end && signature[i] == '[') {
				++i;
			}
			if (i >= end) {
				break;
			}
			if (signature[i] == 'L') {
				++i;
				while (i < end && signature[i] != ';') {
					++i;
				}
				if (i < end && signature[i] == ';') {
					++i;
				}
			} else {
				++i;
			}
			params.emplace_back(signature.substr(paramStart, i - paramStart));
		}

		return params;
	}

	char getBaseType(const std::string& descriptor) {
		size_t idx = 0;
		while (idx < descriptor.size() && descriptor[idx] == '[') {
			++idx;
		}
		if (idx >= descriptor.size()) {
			return '?';
		}
		return descriptor[idx];
	}

	std::string formatObjectRef(ObjectRef obj) {
		return obj ? obj->toString() : std::string("null");
	}

	std::string formatWideArg(ObjectRef lsb, ObjectRef msb, char baseType) {
		if (lsb == nullptr || msb == nullptr || !lsb->isNumberObject() || !msb->isNumberObject()) {
			return fmt::format("<invalid-wide:{}|{}>", formatObjectRef(lsb), formatObjectRef(msb));
		}

		const uint64_t low = static_cast<uint32_t>(lsb->getValue());
		const uint64_t high = static_cast<uint32_t>(msb->getValue());
		const uint64_t bits = (high << 32) | low;

		if (baseType == 'D') {
			const double value = std::bit_cast<double>(bits);
			return fmt::format("{}", value);
		}

		return fmt::format("{}", static_cast<int64_t>(bits));
	}
}  // namespace

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
                    const std::vector<ObjectRef>& args_, bool static_) {
	if (!_trace_calls) {
		return;
	}
	auto params = parseParameterDescriptors(signature_);
	size_t regIdx = 0;
	std::string args_str = "(";
	if (!static_) {
		if (regIdx < args_.size()) {
			args_str += "this=" + formatObjectRef(args_[regIdx]);
			++regIdx;
		} else {
			args_str += "this=<missing>";
		}
	}

	for (const auto& paramDesc : params) {
		if (args_str.size() > 1) {
			args_str += ", ";
		}
		const char baseType = getBaseType(paramDesc);
		if (baseType == 'J' || baseType == 'D') {
			if (regIdx + 1 < args_.size()) {
				args_str += formatWideArg(args_[regIdx], args_[regIdx + 1], baseType);
				regIdx += 2;
			} else {
				args_str += "<missing-wide>";
				regIdx = args_.size();
			}
		} else {
			if (regIdx < args_.size()) {
				args_str += formatObjectRef(args_[regIdx]);
				++regIdx;
			} else {
				args_str += "<missing>";
			}
		}
	}

	if (regIdx < args_.size()) {
		args_str += ", <extra-regs:";
		for (size_t i = regIdx; i < args_.size(); ++i) {
			if (i > regIdx) {
				args_str += ",";
			}
			args_str += formatObjectRef(args_[i]);
		}
		args_str += ">";
	}
	args_str += ")";

	std::string msg = fmt::format("{} {}.{}{} {}", type_, fmt::styled(class_, fmt::fg(fmt::color::cyan)), fmt::styled(method_, fmt::fg(fmt::color::lawn_green)),
	                              fmt::styled(signature_, fmt::fg(fmt::color::yellow)), fmt::styled(args_str, fmt::fg(fmt::color::magenta)));
	logger.log(Logger::LogLevel::INFO | Logger::LogLevel::ALWAYS, msg);
}
