#include <fmt/format.h>

#include <args.hxx>

#include "class.hpp"
#include "classloader.hpp"
#include "jni.hpp"
#include "loader/apk.hpp"
#include "loader/dex.hpp"
#include "system/logger.hpp"
#include "system/sharedlibrary.hpp"
#include "vm.hpp"

using namespace sandvik;

int main(int argc, char** argv) {
	args::ArgumentParser parser("sandvik", "Dalvik virtual machine");
	args::HelpFlag help(parser, "help", "Display available options", {'h', "help"});

	args::Flag version(parser, "version", "Display the version of this program", {"version"});
	args::CompletionFlag completion(parser, {"complete"});

	args::ValueFlag<std::string> logLevel(parser, "level", "Set the log level (NONE, DEBUG, INFO, WARN, ERROR)", {"log"}, "NONE");
	args::ValueFlag<std::string> dexFile(parser, "file", "Specify the DEX file to load", {"dex"}, "");
	args::ValueFlag<std::string> apkFile(parser, "file", "Specify the APK file to load", {"apk"}, "");
	args::ValueFlag<std::string> mainClass(parser, "classname", "Specify the main class to run", {"main"}, "");
	args::ValueFlag<std::string> runTime(parser, "runtime", "Specify the the runtime path", {"runtime"}, "");
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
		std::cout << "sandvik version 1.0.0" << std::endl;
		return 0;
	}

	logger.setLevel(Logger::LogLevel::INFO);
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

	if (args::get(dexFile).empty() && args::get(apkFile).empty()) {
		std::cerr << "Either --dex or --apk must be specified" << std::endl << std::endl;
		std::cerr << parser;
		return 1;
	}
	if (args::get(mainClass).empty()) {
		std::cerr << "Main class not specified" << std::endl << std::endl;
		std::cerr << parser;
		return 1;
	}

	Vm vm;
	if (!args::get(runTime).empty()) {
		vm.addClassPath(args::get(runTime));
	}
	if (!args::get(dexFile).empty()) {
		vm.loadDex(args::get(dexFile));
	} else if (!args::get(apkFile).empty()) {
		vm.loadApk(args::get(apkFile));
	} else {
		logger.error("No valid file specified");
		return 1;
	}

	logger.ok(" === sandvik ===");
	std::string mainClassValue = args::get(mainClass);
	if (mainClassValue.empty()) {
		logger.error("Main class not specified");
		return 1;
	}
	logger.info(fmt::format("Running main class: {}", mainClassValue));
	std::vector<std::string> args = args::get(positionalArgs);
	vm.run(mainClassValue, args);

	logger.info(" === end ===");
	return 0;
}
