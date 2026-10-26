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
#include <cctype>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>

#include "class.hpp"
#include "disassembler.hpp"
#include "method.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

namespace {
	// console colors of the trace lines
	constexpr uint32_t PACKAGE_COLOR = 0x808080;    // grey
	constexpr uint32_t CLASS_COLOR = 0x00FFFF;      // cyan
	constexpr uint32_t METHOD_COLOR = 0x7CFC00;     // lawn green
	constexpr uint32_t SIGNATURE_COLOR = 0xFFFF00;  // yellow
	constexpr uint32_t TYPE_COLOR = 0xFFFF00;       // yellow, parameter and return types
	constexpr uint32_t STRING_COLOR = 0xFFA07A;     // light salmon, string values
	constexpr uint32_t VALUE_COLOR = 0xFF00FF;      // magenta, numbers, booleans and chars
	constexpr uint32_t NULL_COLOR = 0x808080;       // grey, null
	constexpr uint32_t OBJECT_COLOR = 0x00FFFF;     // cyan, object values (shown by their class)
	constexpr uint32_t BYTES_COLOR = 0x808080;      // grey
	constexpr uint32_t REGISTER_COLOR = 0x87CEFA;   // light sky blue, vN operands
	constexpr uint32_t REFERENCE_COLOR = 0xFFFF00;  // yellow, string@/type@/meth@/field@ operands
	constexpr uint32_t COMMENT_COLOR = 0x808080;    // grey

	/** a trace line built twice: plain (log file, uncolored console) and colored (terminal) */
	class TraceLine {
		public:
			explicit TraceLine(bool colored_) : _colored(colored_) {
			}
			void text(std::string_view text_) {
				_plain.append(text_);
				if (_colored) {
					_color.append(text_);
				}
			}
			void styled(std::string_view text_, uint32_t color_) {
				_plain.append(text_);
				if (_colored) {
					_color += fmt::format(fmt::fg(fmt::rgb(color_)), "{}", text_);
				}
			}
			void bold(std::string_view text_) {
				_plain.append(text_);
				if (_colored) {
					_color += fmt::format(fmt::emphasis::bold, "{}", text_);
				}
			}
			void write(Logger::Kind kind_) const {
				if (_colored) {
					logger.write(kind_, _plain, _color);
				} else {
					logger.write(kind_, _plain);
				}
			}

		private:
			bool _colored;
			std::string _plain;
			std::string _color;
	};

	/** "package.Class" with the package dimmed */
	void appendClass(TraceLine& line_, std::string_view class_) {
		auto pos = class_.rfind('.');
		if (pos != std::string_view::npos) {
			line_.styled(class_.substr(0, pos + 1), PACKAGE_COLOR);
			class_ = class_.substr(pos + 1);
		}
		line_.styled(class_, CLASS_COLOR);
	}

	/** readable type of a descriptor: I -> int, Ljava/lang/String; -> String, [B -> byte[] */
	std::string typeName(std::string_view descriptor_) {
		size_t dims = 0;
		while (dims < descriptor_.size() && descriptor_[dims] == '[') {
			++dims;
		}
		auto base = descriptor_.substr(dims);
		std::string name;
		if (!base.empty() && base[0] == 'L') {
			auto cls = base.substr(1, base.size() >= 2 ? base.size() - 2 : 0);
			auto pos = cls.rfind('/');
			name = std::string(pos == std::string_view::npos ? cls : cls.substr(pos + 1));
		} else if (!base.empty()) {
			switch (base[0]) {
				case 'Z':
					name = "boolean";
					break;
				case 'B':
					name = "byte";
					break;
				case 'C':
					name = "char";
					break;
				case 'S':
					name = "short";
					break;
				case 'I':
					name = "int";
					break;
				case 'J':
					name = "long";
					break;
				case 'F':
					name = "float";
					break;
				case 'D':
					name = "double";
					break;
				case 'V':
					name = "void";
					break;
				default:
					name = std::string(base);
					break;
			}
		}
		for (size_t i = 0; i < dims; ++i) {
			name += "[]";
		}
		return name;
	}

	/** an object value: null, a quoted string or its class */
	void appendObject(TraceLine& line_, ObjectRef obj_) {
		if (obj_ == nullptr || obj_->isNull()) {
			line_.styled("null", NULL_COLOR);
		} else if (obj_->isString()) {
			line_.styled(obj_->toString(), STRING_COLOR);
		} else {
			line_.styled(obj_->toString(), OBJECT_COLOR);
		}
	}

	/** a 32 bit value according to its declared type */
	void appendNarrow(TraceLine& line_, char type_, ObjectRef value_) {
		if (value_ == nullptr || !value_->isNumberObject()) {
			appendObject(line_, value_);
			return;
		}
		const int32_t v = value_->getValue();
		switch (type_) {
			case 'Z':
				line_.styled(v != 0 ? "true" : "false", VALUE_COLOR);
				break;
			case 'C':
				if (v >= 0x20 && v < 0x7F) {
					line_.styled(fmt::format("'{}'", static_cast<char>(v)), VALUE_COLOR);
				} else {
					line_.styled(fmt::format("'\\u{:04x}'", static_cast<uint16_t>(v)), VALUE_COLOR);
				}
				break;
			case 'F':
				line_.styled(fmt::format("{}", std::bit_cast<float>(v)), VALUE_COLOR);
				break;
			default:
				line_.styled(fmt::format("{}", v), VALUE_COLOR);
				break;
		}
	}

	/** a 64 bit value split in two registers */
	void appendWide(TraceLine& line_, char type_, ObjectRef lsb_, ObjectRef msb_) {
		if (lsb_ == nullptr || msb_ == nullptr || !lsb_->isNumberObject() || !msb_->isNumberObject()) {
			line_.styled("<invalid-wide>", NULL_COLOR);
			return;
		}
		const uint64_t bits = (static_cast<uint64_t>(static_cast<uint32_t>(msb_->getValue())) << 32) | static_cast<uint32_t>(lsb_->getValue());
		if (type_ == 'D') {
			line_.styled(fmt::format("{}", std::bit_cast<double>(bits)), VALUE_COLOR);
		} else {
			line_.styled(fmt::format("{}", static_cast<int64_t>(bits)), VALUE_COLOR);
		}
	}

	/** instruction operands: registers, literals, references and comments get their own color */
	void appendOperands(TraceLine& line_, std::string_view operands_) {
		size_t i = 0;
		while (i < operands_.size()) {
			if (operands_.substr(i).starts_with("//")) {
				line_.styled(operands_.substr(i), COMMENT_COLOR);
				return;
			}
			const char c = operands_[i];
			if (c == ' ' || c == ',' || c == '{' || c == '}') {
				line_.text(operands_.substr(i, 1));
				++i;
				continue;
			}
			size_t end = i;
			while (end < operands_.size() && operands_[end] != ' ' && operands_[end] != ',' && operands_[end] != '}') {
				++end;
			}
			auto token = operands_.substr(i, end - i);
			if (token.size() > 1 && token[0] == 'v' && std::isdigit(static_cast<unsigned char>(token[1]))) {
				line_.styled(token, REGISTER_COLOR);
			} else if (token.find('@') != std::string_view::npos) {
				line_.styled(token, REFERENCE_COLOR);
			} else {
				line_.styled(token, VALUE_COLOR);
			}
			i = end;
		}
	}

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

}  // namespace

Trace::Trace() : _trace_instructions(false), _trace_calls(false), _disassembler(std::make_unique<Disassembler>()) {
}

void Trace::enableInstructionTrace(bool enable_) {
	_trace_instructions = enable_;
}
void Trace::enableCallTrace(bool enable_) {
	_trace_calls = enable_;
}

void Trace::logInstruction(const uint64_t pc_, const Method& method_, const uint8_t* bytecode_) {
	if (!_trace_instructions) {
		return;
	}
	// "<mnemonic> <operands>   : <bytes>" padded to 80 columns, the bytes have no ':'
	auto inst = fmt::format("{:<80}", _disassembler->disassemble(bytecode_));
	auto colon = inst.rfind(':');
	auto disasm = std::string_view(inst).substr(0, colon == std::string::npos ? inst.size() : colon);
	auto bytes = colon == std::string::npos ? std::string_view() : std::string_view(inst).substr(colon);
	auto space = disasm.find(' ');
	auto mnemonic = disasm.substr(0, space == std::string_view::npos ? disasm.size() : space);

	TraceLine line(logger.isColoredConsole());
	line.styled(fmt::format("{:04x}", pc_ / 2), BYTES_COLOR);
	line.text(": ");
	line.bold(mnemonic);
	appendOperands(line, disasm.substr(mnemonic.size()));
	line.styled(bytes, BYTES_COLOR);
	line.text(" ");
	appendClass(line, method_.getClass().getFullname());
	line.text("::");
	line.styled(method_.getName(), METHOD_COLOR);
	line.styled(method_.getSignature(), SIGNATURE_COLOR);
	line.write(Logger::Kind::INSTRUCTION);
}

void Trace::logCall(const std::string& type_, const std::string& class_, const std::string& method_, const std::string& signature_,
                    const std::vector<ObjectRef>& args_, bool static_) {
	if (!_trace_calls) {
		return;
	}
	// invoke-virtual java.util.HashMap.put(this=java.util.HashMap, Object="key", int=3) -> Object
	TraceLine line(logger.isColoredConsole());
	line.text(type_);
	line.text(" ");
	appendClass(line, class_);
	line.text(".");
	line.styled(method_, METHOD_COLOR);
	line.text("(");

	size_t regIdx = 0;
	bool first = true;
	if (!static_) {
		line.text("this=");
		appendObject(line, regIdx < args_.size() ? args_[regIdx] : nullptr);
		++regIdx;
		first = false;
	}
	for (const auto& param : parseParameterDescriptors(signature_)) {
		if (!first) {
			line.text(", ");
		}
		first = false;
		line.styled(typeName(param), TYPE_COLOR);
		line.text("=");
		const char type = param[0];
		if (type == 'J' || type == 'D') {
			appendWide(line, type, regIdx < args_.size() ? args_[regIdx] : nullptr, regIdx + 1 < args_.size() ? args_[regIdx + 1] : nullptr);
			regIdx += 2;
		} else if (type == 'L' || type == '[') {
			appendObject(line, regIdx < args_.size() ? args_[regIdx] : nullptr);
			++regIdx;
		} else {
			appendNarrow(line, type, regIdx < args_.size() ? args_[regIdx] : nullptr);
			++regIdx;
		}
	}
	for (; regIdx < args_.size(); ++regIdx) {
		line.text(", <extra>=");
		appendObject(line, args_[regIdx]);
	}
	line.text(")");

	auto ret = signature_.find(')');
	if (ret != std::string::npos && ret + 1 < signature_.size() && signature_[ret + 1] != 'V') {
		line.text(" -> ");
		line.styled(typeName(std::string_view(signature_).substr(ret + 1)), TYPE_COLOR);
	}
	line.write(Logger::Kind::CALL);
}
