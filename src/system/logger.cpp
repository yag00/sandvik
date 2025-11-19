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

#include <fmt/color.h>
#include <fmt/format.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

using namespace sandvik;

Logger::Logger() {
}

Logger::~Logger() {
	if (_file.is_open()) {
		_file.flush();
		_file.close();
	}
}

void Logger::logToConsole(bool enable_) {
	_stdout = enable_;
}

void Logger::logToFile(const std::string &filename_) {
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
	_threads[tid_] = name_;
}

void Logger::removeThread(std::thread::id tid_) {
	_threads.erase(tid_);
}

void Logger::displayThreadName(bool enable_) {
	_threadname = enable_;
}

Logger::LogLevel Logger::getLevel() const {
	return _level;
}
void Logger::setLevel(Logger::LogLevel level_) {
	_level = level_;
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

void Logger::debug(const char *msg_, ...) {
	char msg[1024] = {'\0'};
	va_list args;
	va_start(args, msg_);
	vsprintf((char *)&msg, msg_, args);
	va_end(args);
	debug(std::string(msg));
}

void Logger::info(const char *msg_, ...) {
	char msg[1024] = {'\0'};
	va_list args;
	va_start(args, msg_);
	vsprintf((char *)&msg, msg_, args);
	va_end(args);
	info(std::string(msg));
}

void Logger::warning(const char *msg_, ...) {
	char msg[1024] = {'\0'};
	va_list args;
	va_start(args, msg_);
	vsprintf((char *)&msg, msg_, args);
	va_end(args);
	warning(std::string(msg));
}

void Logger::error(const char *msg_, ...) {
	char msg[1024] = {'\0'};
	va_list args;
	va_start(args, msg_);
	vsprintf((char *)&msg, msg_, args);
	va_end(args);
	error(std::string(msg));
}

void Logger::ok(const char *msg_, ...) {
	char msg[1024] = {'\0'};
	va_list args;
	va_start(args, msg_);
	vsprintf((char *)&msg, msg_, args);
	va_end(args);
	ok(std::string(msg));
}

void Logger::color(uint32_t color_, char marker_, const char *msg_, ...) {
	char msg[1024] = {'\0'};
	va_list args;
	va_start(args, msg_);
	vsprintf((char *)&msg, msg_, args);
	va_end(args);
	color(color_, marker_, std::string(msg));
}

void Logger::color(uint32_t color_, char marker_, const std::string &msg_) {
	if (LogLevel::INFO < _level) {
		return;
	}

	if (_file.is_open()) {
		std::string rawlog;
		if (_time) {
			rawlog += fmt::format("[{}] ", getTime());
		}
		rawlog += fmt::format("[{}] {}", marker_, msg_);
		_file << rawlog << std::endl;
	}

	if (_stdout) {
		if (_time) {
			fmt::print(fmt::fg(fmt::color::white), "[{}] ", getTime());
		}
		fmt::print(fmt::fg(fmt::rgb(color_)), "[{}] {}", marker_, msg_);
		fmt::print(fmt::fg(fmt::color::white), "\n");
	}
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
	if (level < _level) {
		return;
	}
	// Mask out ALWAYS bit
	level &= 0xF;

	std::string threadname = "";
	if (_threadname) {
		auto thread = _threads.find(std::this_thread::get_id());
		if (thread != _threads.end()) {
			// Prefix message with thread name
			threadname = fmt::format("[{}] ", thread->second);
		}
	}

	if (_file.is_open()) {
		std::string rawlog;
		if (_time) {
			rawlog += fmt::format("[{}] ", getTime());
		}
		rawlog += threadname;
		switch (level) {
			case LogLevel::INFO:
				rawlog += fmt::format("[*] {}", msg);
				break;
			case LogLevel::DEBUG:
				rawlog += fmt::format("{}", msg);
				break;
			case LogLevel::WARNING:
				rawlog += fmt::format("[w] {}", msg);
				break;
			case LogLevel::ERROR:
				rawlog += fmt::format("[!] {}", msg);
				break;
			case LogLevel::OK:
				rawlog += fmt::format("[+] {}", msg);
				break;
			default:
				break;
		}
		std::scoped_lock lock(_mutex);
		_file << rawlog << std::endl;
	}

	if (_stdout) {
		std::scoped_lock lock(_mutex);
		if (_time) {
			fmt::print(fmt::fg(fmt::color::white), "[{}] ", getTime());
		}
		fmt::print(fmt::fg(fmt::color::white), "{}", threadname);
		switch (level) {
			case LogLevel::INFO:
				fmt::print(fmt::fg(fmt::color::white), "[*] {}", msg);
				break;
			case LogLevel::DEBUG:
				fmt::print(fmt::fg(fmt::color::deep_pink), "{}", msg);
				break;
			case LogLevel::WARNING:
				fmt::print(fmt::fg(fmt::color::yellow), "[w] {}", msg);
				break;
			case LogLevel::ERROR:
				fmt::print(fmt::fg(fmt::color::red) | fmt::emphasis::bold, "[!] {}", msg);
				break;
			case LogLevel::OK:
				fmt::print(fmt::fg(fmt::color::lawn_green), "[+] {}", msg);
				break;
			default:
				break;
		}
		fmt::print(fmt::fg(fmt::color::white), "\n");
	}
}
