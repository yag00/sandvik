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

#include "logger.hpp"

#include <unistd.h>

#include <fmt/color.h>
#include <fmt/format.h>

#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <string>

using namespace sandvik;

namespace {
	/** marker and console color of a kind of log line */
	struct KindStyle {
			std::string_view marker;
			uint32_t color;
			bool bold;
	};

	/** indexed by Logger::Kind */
	constexpr std::array<KindStyle, 10> kindStyles = {{
	    {"[v]  ", 0x808080, false},  // VERBOSE: grey, indented under the instruction trace
	    {"[t]", 0x9370DB, false},    // TRACE: medium purple
	    {"[d]", 0xFF1493, false},    // DEBUG: deep pink
	    {"[*]", 0xFFFFFF, false},    // INFO: white
	    {"[w]", 0xFFFF00, false},    // WARNING: yellow
	    {"[+]", 0x7CFC00, false},    // OK: lawn green
	    {"[!]", 0xFF0000, true},     // ERROR: bold red
	    {"[i]", 0x6495ED, false},    // INSTRUCTION: cornflower blue
	    {"[c]", 0x00CED1, false},    // CALL: dark turquoise
	    {"[app]", 0xFFA500, false},  // APP: orange
	}};

	/** LogLevel and Kind share the same order for the VM levels */
	constexpr Logger::Kind kindOf(Logger::LogLevel level_) {
		return static_cast<Logger::Kind>(static_cast<int>(level_));
	}

	/** name of the current thread, set by the thread itself through Logger::addThread */
	thread_local std::string currentThreadName;
}  // namespace

Logger::Logger() {
	const char *noColor = std::getenv("NO_COLOR");
	_colors = isatty(fileno(stderr)) && (noColor == nullptr || noColor[0] == '\0');
}

Logger::~Logger() {
	if (_file.is_open()) {
		try {
			_file.flush();
			_file.close();
		} catch (...) {
			// Best-effort cleanup: a destructor must never throw.
		}
	}
}

void Logger::logToConsole(bool enable_) {
	_console = enable_;
}

void Logger::useColors(bool enable_) {
	_colors = enable_;
}

void Logger::logToFile(const std::string &filename_) {
	std::scoped_lock lock(_mutex);
	if (_file.is_open()) {
		_file.flush();
		_file.close();
	}
	_file.open(filename_.c_str(), std::ofstream::out | std::ofstream::app);
	if (!_file.is_open()) {
		throw std::runtime_error("Can't open log file :" + filename_ + "!");
	}
}

void Logger::addThread(std::thread::id tid_, const std::string &name_) {
	if (tid_ == std::this_thread::get_id()) {
		currentThreadName = name_;
		return;
	}
	std::scoped_lock lock(_threadsMutex);
	_threads[tid_] = name_;
}

void Logger::removeThread(std::thread::id tid_) {
	if (tid_ == std::this_thread::get_id()) {
		currentThreadName.clear();
	}
	std::scoped_lock lock(_threadsMutex);
	_threads.erase(tid_);
}

void Logger::displayThreadName(bool enable_) {
	_logThreadName = enable_;
}

std::string Logger::getThreadName() const {
	if (!currentThreadName.empty()) {
		return currentThreadName;
	}
	std::scoped_lock lock(const_cast<std::mutex &>(_threadsMutex));
	auto thread = _threads.find(std::this_thread::get_id());
	return thread != _threads.end() ? thread->second : std::string();
}

Logger::LogLevel Logger::getLevel() const {
	return _level.load(std::memory_order_relaxed);
}
void Logger::setLevel(Logger::LogLevel level_) {
	_level.store(level_, std::memory_order_relaxed);
}

Logger::LogLevel Logger::getAppLevel() const {
	return _appLevel.load(std::memory_order_relaxed);
}
void Logger::setAppLevel(Logger::LogLevel level_) {
	_appLevel.store(level_, std::memory_order_relaxed);
}

bool Logger::isDisplayingTime() const {
	return _time;
}

void Logger::displayTime(bool enable) {
	_time = enable;
}

void Logger::info(const std::string &msg) {
	log(LogLevel::INFO, msg);
}

void Logger::debug(const std::string &msg) {
	log(LogLevel::DEBUG, msg);
}

void Logger::warning(const std::string &msg) {
	log(LogLevel::WARNING, msg);
}

void Logger::error(const std::string &msg) {
	log(LogLevel::ERROR, msg);
}

void Logger::ok(const std::string &msg) {
	log(LogLevel::OK, msg);
}

void Logger::color(uint32_t color_, char marker_, const std::string &msg_) {
	if (!isEnabled(LogLevel::INFO)) {
		return;
	}
	const char marker[] = {'[', marker_, ']'};
	writeLine(std::string_view(marker, sizeof(marker)), color_, false, false, msg_, msg_);
}

std::string Logger::getTime() const {
	auto now = std::chrono::system_clock::now();
	std::time_t timestamp = std::chrono::system_clock::to_time_t(now);
	char buf[128];
	ctime_r(&timestamp, buf);
	std::string timeStr(buf);
	timeStr.pop_back();  // Remove newline character from the time string
	return timeStr;
}

void Logger::log(LogLevel level, const std::string &msg) {
	if (level == LogLevel::NONE || !isEnabled(level)) {
		return;
	}
	write(kindOf(level), msg);
}

void Logger::vlog(LogLevel level_, fmt::string_view fmt_, fmt::format_args args_) {
	if (level_ == LogLevel::NONE) {
		return;
	}
	fmt::memory_buffer msg;
	fmt::vformat_to(std::back_inserter(msg), fmt_, args_);
	write(kindOf(level_), std::string_view(msg.data(), msg.size()));
}

void Logger::write(Kind kind_, std::string_view msg_) {
	write(kind_, msg_, msg_);
}

void Logger::write(Kind kind_, std::string_view msg_, std::string_view coloredMsg_) {
	const auto &style = kindStyles[static_cast<size_t>(kind_)];
	writeLine(style.marker, style.color, style.bold, kind_ == Kind::WARNING || kind_ == Kind::ERROR, msg_, coloredMsg_);
}

void Logger::app(char priority_, std::string_view tag_, std::string_view msg_) {
	LogLevel level = LogLevel::INFO;
	switch (priority_) {
		case 'V':
			level = LogLevel::VERBOSE;
			break;
		case 'D':
			level = LogLevel::DEBUG;
			break;
		case 'W':
			level = LogLevel::WARNING;
			break;
		case 'E':
		case 'F':
			level = LogLevel::ERROR;
			break;
		default:
			break;
	}
	if (!isAppEnabled(level)) {
		return;
	}
	const auto &style = kindStyles[static_cast<size_t>(Kind::APP)];
	const bool important = level >= LogLevel::WARNING;
	// like logcat, a multi-line message (e.g. a stack trace) gives one prefixed line per message line
	size_t pos = 0;
	while (pos <= msg_.size()) {
		auto end = msg_.find('\n', pos);
		if (end == std::string_view::npos) {
			end = msg_.size();
		}
		auto line = msg_.substr(pos, end - pos);
		if (!line.empty() || end < msg_.size()) {
			auto plain = fmt::format("{}/{}: {}", priority_, tag_, line);
			if (important && isColoredConsole()) {
				const auto &levelStyle = kindStyles[static_cast<size_t>(level == LogLevel::WARNING ? Kind::WARNING : Kind::ERROR)];
				auto color = fmt::fg(fmt::rgb(levelStyle.color));
				if (levelStyle.bold) {
					color |= fmt::emphasis::bold;
				}
				writeLine(style.marker, style.color, style.bold, false, plain, fmt::format(color, "{}", plain));
			} else {
				writeLine(style.marker, style.color, style.bold, important, plain, plain);
			}
		}
		pos = end + 1;
	}
}

void Logger::writeLine(std::string_view marker_, uint32_t color_, bool bold_, bool important_, std::string_view msg_, std::string_view coloredMsg_) {
	// Build the whole line before taking the lock, then write it at once
	fmt::memory_buffer head;
	if (_time) {
		fmt::format_to(std::back_inserter(head), "[{}] ", getTime());
	}
	if (_logThreadName) {
		auto name = getThreadName();
		if (!name.empty()) {
			fmt::format_to(std::back_inserter(head), "[{}] ", name);
		}
	}

	fmt::memory_buffer console;
	if (_console) {
		console.append(head);
		if (_colors) {
			auto style = fmt::fg(fmt::rgb(color_));
			if (bold_) {
				style |= fmt::emphasis::bold;
			}
			if (important_) {
				// warnings and errors must stand out: color the whole line
				fmt::format_to(std::back_inserter(console), style, "{} {}", marker_, msg_);
			} else {
				// only the marker is colored, the message may carry its own colors
				fmt::format_to(std::back_inserter(console), style, "{}", marker_);
				fmt::format_to(std::back_inserter(console), " {}", coloredMsg_);
			}
		} else {
			fmt::format_to(std::back_inserter(console), "{} {}", marker_, msg_);
		}
		console.push_back('\n');
	}

	std::scoped_lock lock(_mutex);
	if (_file.is_open()) {
		_file.write(head.data(), static_cast<std::streamsize>(head.size()));
		_file << marker_ << ' ' << msg_ << '\n';
		// Keep warnings and errors even if the VM crashes right after
		if (important_) {
			_file.flush();
		}
	}
	if (_console) {
		std::fwrite(console.data(), 1, console.size(), stderr);
	}
}
