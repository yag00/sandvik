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

#include <fmt/format.h>

#include <args.hxx>
#include <chrono>
#include <filesystem>
#include <map>
#include <optional>

#include "class.hpp"
#include "classloader.hpp"
#include "disassembler.hpp"
#include "jni.hpp"
#include "loader/apk.hpp"
#include "loader/dex.hpp"
#include "loader/getprop.hpp"
#include "system/env_var.hpp"
#include "system/logger.hpp"
#include "system/sharedlibrary.hpp"
#include "trace.hpp"
#include "version.hpp"
#include "vfs.hpp"
#include "vm.hpp"
#include "zygote.hpp"

using namespace sandvik;

namespace {
	/** parse a --log / --app-log value, TRACE only exists for the VM */
	std::optional<Logger::LogLevel> parseLogLevel(const std::string& level_, bool vm_) {
		static const std::map<std::string, Logger::LogLevel, std::less<>> levels = {
		    {"NONE", Logger::LogLevel::NONE}, {"VERBOSE", Logger::LogLevel::VERBOSE}, {"TRACE", Logger::LogLevel::TRACE}, {"DEBUG", Logger::LogLevel::DEBUG},
		    {"INFO", Logger::LogLevel::INFO}, {"WARN", Logger::LogLevel::WARNING},    {"ERROR", Logger::LogLevel::ERROR}};
		auto it = levels.find(level_);
		if (it == levels.end() || (!vm_ && it->second == Logger::LogLevel::TRACE)) {
			return std::nullopt;
		}
		return it->second;
	}
}  // namespace

int main(int argc, char** argv) {
	args::ArgumentParser parser("sandvik", "Dalvik virtual machine");
	args::HelpFlag help(parser, "help", "Display available options", {'h', "help"});

	args::Flag version(parser, "version", "Display the version of this program", {"version"});
	args::CompletionFlag completion(parser, {"complete"});

	args::ValueFlag<std::string> logLevel(parser, "level", "Set the log level (NONE, VERBOSE, TRACE, DEBUG, INFO, WARN, ERROR)", {"log"}, "NONE");
	args::ValueFlag<std::string> appLogLevel(parser, "level", "Set the log level of the Android application logs (NONE, VERBOSE, DEBUG, INFO, WARN, ERROR)",
	                                         {"app-log"}, "INFO");
	args::ValueFlag<std::string> logFile(parser, "logfile", "Set the log output file", {"logfile"}, "");
	args::Flag noConsole(parser, "no-console", "Disable console output", {"no-console"});
	args::Flag displayThread(parser, "thread", "Display thread name in logs", {'t', "display-thread"});
	args::Flag instructiontrace(parser, "instruction", "Instruction trace", {'i', "instructions"});
	args::Flag calltrace(parser, "calltrace", "Call trace", {'c', "calltrace"});
	args::ValueFlagList<std::string> dexFiles(parser, "file", "Specify the DEX files to load", {"dex"});
	args::ValueFlag<std::string> androidVersion(parser, "version", "Specify the Android version to emulate (e.g., 12.0.0)", {"android-version"}, "");
	args::ValueFlag<std::string> jarDir(parser, "dir", "Directory containing runtime JARs (loaded in order)", {"jar-dir"});
	args::ValueFlagList<std::string> jarFiles(parser, "file", "Specify the Jar files to load", {"jar"});
	args::ValueFlag<std::string> apkFile(parser, "file", "Specify the APK file to load", {"apk"}, "");
	args::ValueFlag<std::string> androidRoot(parser, "dir", "Specify the virtual Android filesystem root directory", {"android-root"}, "");
	args::ValueFlag<std::string> propFile(parser, "file", "Specify an Android getprop output file to preload properties", {"prop"}, "");
	args::ValueFlag<std::string> mainClass(parser, "classname", "Specify the main class to run", {"main"}, "");
	args::PositionalList<std::string> positionalArgs(parser, "args", "Positional arguments for the java program");

	try {
		parser.ParseCLI(argc, argv);
	} catch (args::Help&) {
		std::cout << parser;
		return 0;
	} catch (args::Completion& e) {
		std::cout << e.what();
		return 0;
	} catch (args::ParseError& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 0;
	} catch (args::ValidationError& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 0;
	}

	if (version) {
		fmt::print("Sandvik version {}-{}\n", sandvik::version::getVersion(), sandvik::version::getShortCommit());
		fmt::print("Sandvik date {}\n", sandvik::version::getBuildDate());
		fmt::print("Sandvik commit {}\n", sandvik::version::getCommit());
		return 0;
	}

	logger.setLevel(Logger::LogLevel::INFO);
	if (args::get(logFile) != "") {
		logger.logToFile(args::get(logFile));
	}
	if (noConsole) {
		logger.logToConsole(false);
	}
	logger.displayThreadName(args::get(displayThread));

	if (logLevel) {
		auto level = parseLogLevel(args::get(logLevel), true);
		if (!level.has_value()) {
			std::cerr << "Invalid log level: " << args::get(logLevel) << std::endl;
			std::cerr << "Valid log levels are: NONE, VERBOSE, TRACE, DEBUG, INFO, WARN, ERROR" << std::endl << std::endl;
			std::cerr << parser;
			return 1;
		}
		logger.setLevel(level.value());
		if (static_cast<int>(level.value()) < SANDVIK_LOG_ACTIVE_LEVEL) {
			logger.fwarning("log level {} is not compiled in this build, reconfigure with --debug to get these messages", args::get(logLevel));
		}
	}
	if (appLogLevel) {
		auto level = parseLogLevel(args::get(appLogLevel), false);
		if (!level.has_value()) {
			std::cerr << "Invalid application log level: " << args::get(appLogLevel) << std::endl;
			std::cerr << "Valid application log levels are: NONE, VERBOSE, DEBUG, INFO, WARN, ERROR" << std::endl << std::endl;
			std::cerr << parser;
			return 1;
		}
		logger.setAppLevel(level.value());
	}
	logger.fok("=== sandvik {}-{} ===", sandvik::version::getVersion(), sandvik::version::getShortCommit());

	trace.enableInstructionTrace(args::get(instructiontrace));
	trace.enableCallTrace(args::get(calltrace));

	if (args::get(mainClass).empty() && args::get(apkFile).empty()) {
		std::cerr << "Main class not specified" << std::endl << std::endl;
		std::cerr << parser;
		return 1;
	}

	Vm vm;

	// Determine base file directory for resources based on Android version
	auto baseFileDir = std::optional<std::string>();
	if (!args::get(androidVersion).empty()) {
		baseFileDir = "android/" + args::get(androidVersion) + "/";
	}

	// Load Device Properties from getprop output file if specified
	if (!args::get(propFile).empty()) {
		GetProp::getInstance().loadFromFile(args::get(propFile));
	} else if (baseFileDir.has_value()) {
		GetProp::getInstance().loadFromFile(baseFileDir.value() + "getprop.prop");
	}

	// Load DEX files
	for (const auto& dexFile : args::get(dexFiles)) {
		vm.loadDex(dexFile);
	}
	// Load JAR (containing DEX) files by directory or by explicit file list
	if (!args::get(jarDir).empty()) {
		logger.finfo("Runtime: {}", args::get(jarDir));
		vm.loadRtByDir(args::get(jarDir));
	} else if (baseFileDir.has_value()) {
		logger.finfo("Runtime: Android {} ({}bin)", args::get(androidVersion), baseFileDir.value());
		vm.loadRtByDir(baseFileDir.value() + "bin");
	}
	if (!args::get(jarFiles).empty()) {
		logger.finfo("Runtime: {} jar file(s)", args::get(jarFiles).size());
		for (const auto& jarFile : args::get(jarFiles)) {
			vm.loadRtByFile(jarFile);
		}
	}

	// Load APK file
	if (!args::get(apkFile).empty()) {
		logger.finfo("APK: {}", args::get(apkFile));
		vm.loadApk(args::get(apkFile));
	}

	// Set Android Virtual Filesystem Root Directory
	if (!args::get(androidRoot).empty()) {
		VFS::setAndroidRoot(args::get(androidRoot));
	} else if (baseFileDir.has_value()) {
		VFS::setAndroidRoot(baseFileDir.value() + "fs");
	}

	setupZygoteSocket();

	// Run the VM
	const auto start = std::chrono::steady_clock::now();
	try {
		if (!args::get(apkFile).empty()) {
			// Running APK file, main class is extracted from the APK manifest
			vm.run();
		} else {
			// Running normal class
			std::vector<std::string> args = args::get(positionalArgs);
			vm.run(args::get(mainClass), args);
		}
	} catch (const std::exception& e) {
		logger.setLevel(Logger::LogLevel::INFO);
		logger.ferror("{}", e.what());
		return 1;
	}

	const std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - start;
	logger.finfo("=== end ({:.3f}s) ===", elapsed.count());
	return 0;
}
