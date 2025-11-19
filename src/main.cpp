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

#include "class.hpp"
#include "classloader.hpp"
#include "disassembler.hpp"
#include "jni.hpp"
#include "loader/apk.hpp"
#include "loader/dex.hpp"
#include "system/logger.hpp"
#include "system/sharedlibrary.hpp"
#include "trace.hpp"
#include "version.hpp"
#include "vm.hpp"

using namespace sandvik;

int main(int argc, char** argv) {
	args::ArgumentParser parser("sandvik", "Dalvik virtual machine");
	args::HelpFlag help(parser, "help", "Display available options", {'h', "help"});

	args::Flag version(parser, "version", "Display the version of this program", {"version"});
	args::CompletionFlag completion(parser, {"complete"});

	args::ValueFlag<std::string> logLevel(parser, "level", "Set the log level (NONE, DEBUG, INFO, WARN, ERROR)", {"log"}, "NONE");
	args::ValueFlag<std::string> logFile(parser, "logfile", "Set the log output file", {"logfile"}, "");
	args::Flag noConsole(parser, "no-console", "Disable console output", {"no-console"});
	args::Flag instructiontrace(parser, "instruction", "Instruction trace", {'i', "instructions"});
	args::Flag calltrace(parser, "calltrace", "Call trace", {'c', "calltrace"});
	args::ValueFlagList<std::string> dexFiles(parser, "file", "Specify the DEX files to load", {"dex"});
	args::ValueFlagList<std::string> jarFiles(parser, "file", "Specify the Jar files to load", {"jar"});
	args::ValueFlag<std::string> apkFile(parser, "file", "Specify the APK file to load", {"apk"}, "");
	args::ValueFlag<std::string> mainClass(parser, "classname", "Specify the main class to run", {"main"}, "");
	args::ValueFlag<std::string> runTime(parser, "runtime", "Specify path to override the default java runtime", {"runtime"}, "");
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
	logger.fok(" === sandvik {}-{} ===", sandvik::version::getVersion(), sandvik::version::getShortCommit());
	if (logLevel) {
		std::string level = args::get(logLevel);
		if (level == "NONE") {
			logger.setLevel(Logger::LogLevel::NONE);
		} else if (level == "DEBUG") {
			logger.setLevel(Logger::LogLevel::DEBUG);
		} else if (level == "INFO") {
			logger.setLevel(Logger::LogLevel::INFO);
		} else if (level == "WARN") {
			logger.setLevel(Logger::LogLevel::WARNING);
		} else if (level == "ERROR") {
			logger.setLevel(Logger::LogLevel::ERROR);
		} else {
			std::cerr << "Invalid log level: " << level << std::endl;
			std::cerr << "Valid log levels are: NONE, DEBUG, INFO, WARN, ERROR" << std::endl << std::endl;
			std::cerr << parser;
			return 1;
		}
	}

	trace.enableInstructionTrace(args::get(instructiontrace));
	trace.enableCallTrace(args::get(calltrace));

	if (args::get(mainClass).empty() && args::get(apkFile).empty()) {
		std::cerr << "Main class not specified" << std::endl << std::endl;
		std::cerr << parser;
		return 1;
	}

	Vm vm;
	// load runtime
	vm.loadRt(args::get(runTime));
	// load dex files
	for (const auto& dexFile : args::get(dexFiles)) {
		vm.loadDex(dexFile);
	}
	// load jar (containing dex) files
	for (const auto& jarFile : args::get(jarFiles)) {
		vm.loadRt(jarFile);
	}
	// load apk file
	if (!args::get(apkFile).empty()) {
		vm.loadApk(args::get(apkFile));
	}
	// run the VM
	try {
		if (!args::get(apkFile).empty()) {
			// running APK file, main class is extracted from the APK manifest
			vm.run();
		} else {
			// running normal class
			std::vector<std::string> args = args::get(positionalArgs);
			vm.run(args::get(mainClass), args);
		}
	} catch (const std::exception& e) {
		logger.setLevel(Logger::LogLevel::INFO);
		logger.error(e.what());
		return 1;
	}

	logger.info(" === end ===");
	return 0;
}
