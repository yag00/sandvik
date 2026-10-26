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

#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <stdint.h>

#include <fmt/format.h>

#include <atomic>
#include <fstream>
#include <map>
#include <mutex>
#include <string>
#include <string_view>
#include <system/singleton.hpp>
#include <thread>

#define logger ::sandvik::getLogger()

/** Lowest log level compiled in (0: VERBOSE, 1: TRACE, 2: DEBUG, ...). Messages below are removed at compile time.
 * VERBOSE and TRACE are only available in debug builds, DEBUG stays available (runtime checked) in release builds.
 */
#ifdef __debug__
#define SANDVIK_LOG_ACTIVE_LEVEL 0
#else
#define SANDVIK_LOG_ACTIVE_LEVEL 2
#endif

namespace sandvik {
	/** @brief Logger class
	 *
	 * Log lines are written to stderr (stdout is left to the running Java program) and optionally to a file.
	 * On a terminal, each kind of line has its own color.
	 */
	class Logger : public Singleton<Logger> {
		public:
			/** enum for log level */
			enum class LogLevel { VERBOSE = 0, TRACE, DEBUG, INFO, WARNING, OK, ERROR, NONE };
			/** enum for log line kind, each kind has its own marker and color */
			enum class Kind {
				VERBOSE,      ///< [v] VM verbose message (register level details)
				TRACE,        ///< [t] VM trace message
				DEBUG,        ///< [d] VM debug message
				INFO,         ///< [*] VM info message
				WARNING,      ///< [w] VM warning message
				OK,           ///< [+] VM success message
				ERROR,        ///< [!] VM error message
				INSTRUCTION,  ///< [i] instruction trace (-i)
				CALL,         ///< [c] call trace (-c)
				APP           ///< [app] log of the running Android application
			};

			/** add thread to logger : log messages from this thread will be prefixed with thread name
			 * @param tid_ thread id
			 * @param name_ thread name
			 */
			void addThread(std::thread::id tid_, const std::string &name_);
			/** remove thread from logger : log messages from this thread won't be prefixed with thread name
			 * @param tid_ thread id
			 */
			void removeThread(std::thread::id tid_);
			/** prefix log message with thread name
			 * @param enable_ enable/disable thread name prefix
			 */
			void displayThreadName(bool enable_);

			/** log to file
			 * @param filename_ filename for log
			 */
			void logToFile(const std::string &filename_);
			/** enable/disable log output to console (stderr)
			 * @param enable_ enable/disable log on console
			 */
			void logToConsole(bool enable_);
			/** enable/disable colors on console, enabled by default when stderr is a terminal and NO_COLOR is not set
			 * @param enable_ enable/disable colors
			 */
			void useColors(bool enable_);

			/** get current log level
			 * @return log level
			 */
			LogLevel getLevel() const;
			/** set log level
			 * @param level_ log level
			 */
			void setLevel(LogLevel level_);
			/** check if a log level is enabled, cheap enough to be used on hot paths
			 * @param level_ log level
			 * @return true if messages of this level are logged
			 */
			static bool isEnabled(LogLevel level_) {
				return level_ >= _level.load(std::memory_order_relaxed);
			}
			/** set the log level of the running application (Android Log, System.log), independent of the VM log level
			 * @param level_ log level
			 */
			void setAppLevel(LogLevel level_);
			/** get the log level of the running application
			 * @return log level
			 */
			LogLevel getAppLevel() const;
			/** check if an application log level is enabled
			 * @param level_ log level
			 * @return true if application messages of this level are logged
			 */
			static bool isAppEnabled(LogLevel level_) {
				return level_ >= _appLevel.load(std::memory_order_relaxed);
			}
			/** check if console lines are colored: callers can then build a colored variant of their message
			 * @return true if the console is enabled and colored
			 */
			bool isColoredConsole() const {
				return _console && _colors;
			}
			/** return true if log message is prefixed with time
			 * @return if log message is prefixed with time
			 */
			bool isDisplayingTime() const;
			/** prefix log message with time
			 * @param enable_ enable/disable time prefix
			 */
			void displayTime(bool enable_);

			/** log info message
			 * @param msg_ message
			 */
			void info(const std::string &msg_);
			/** log debug message
			 * @param msg_ message
			 */
			void debug(const std::string &msg_);
			/** log warning message
			 * @param msg_ message
			 */
			void warning(const std::string &msg_);
			/** log error message
			 * @param msg_ message
			 */
			void error(const std::string &msg_);
			/** log ok message
			 * @param msg_ message
			 */
			void ok(const std::string &msg_);

			/** log info message with custom color and marker
			 * @param color_ 24 bit hex color
			 * @param marker_ marker character
			 * @param msg_ message
			 */
			void color(uint32_t color_, char marker_, const std::string &msg_);

			/** formatted log message: the level is checked before formatting, and nothing is compiled below SANDVIK_LOG_ACTIVE_LEVEL
			 * @param level_ log level
			 * @param fmt_ format string
			 * @param args_ arguments
			 */
			template <typename... Args>
			inline void flog(LogLevel level_, fmt::format_string<Args...> fmt_, Args &&...args_) {
				if (static_cast<int>(level_) >= SANDVIK_LOG_ACTIVE_LEVEL && isEnabled(level_)) [[unlikely]] {
					vlog(level_, fmt_.get(), fmt::make_format_args(args_...));
				}
			}
			/** formatted verbose log message
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			inline void fverbose(fmt::format_string<Args...> fmt_str, Args &&...args) {
				flog(LogLevel::VERBOSE, fmt_str, std::forward<Args>(args)...);
			}
			/** formatted trace log message
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			inline void ftrace(fmt::format_string<Args...> fmt_str, Args &&...args) {
				flog(LogLevel::TRACE, fmt_str, std::forward<Args>(args)...);
			}
			/** formatted debug log message
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			inline void fdebug(fmt::format_string<Args...> fmt_str, Args &&...args) {
				flog(LogLevel::DEBUG, fmt_str, std::forward<Args>(args)...);
			}
			/** formatted info log message
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			inline void finfo(fmt::format_string<Args...> fmt_str, Args &&...args) {
				flog(LogLevel::INFO, fmt_str, std::forward<Args>(args)...);
			}
			/** formatted warning log message
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			inline void fwarning(fmt::format_string<Args...> fmt_str, Args &&...args) {
				flog(LogLevel::WARNING, fmt_str, std::forward<Args>(args)...);
			}
			/** formatted error log message
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			inline void ferror(fmt::format_string<Args...> fmt_str, Args &&...args) {
				flog(LogLevel::ERROR, fmt_str, std::forward<Args>(args)...);
			}
			/** formatted ok log message
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			inline void fok(fmt::format_string<Args...> fmt_str, Args &&...args) {
				flog(LogLevel::OK, fmt_str, std::forward<Args>(args)...);
			}
			/** log message
			 * @param level log level
			 * @param msg message
			 */
			void log(LogLevel level, const std::string &msg);
			/** log formatted message (type erased, used by flog)
			 * @param level_ log level
			 * @param fmt_ format string
			 * @param args_ arguments
			 */
			void vlog(LogLevel level_, fmt::string_view fmt_, fmt::format_args args_);
			/** write a line of the given kind, without level filtering (traces and application logs filter themselves)
			 * @param kind_ kind of line
			 * @param msg_ message
			 */
			void write(Kind kind_, std::string_view msg_);
			/** write a line of the given kind with a colored variant of the message for the console
			 * @param kind_ kind of line
			 * @param msg_ message, written to the log file and to an uncolored console
			 * @param coloredMsg_ message written to a colored console (see isColoredConsole())
			 */
			void write(Kind kind_, std::string_view msg_, std::string_view coloredMsg_);
			/** log a message of the running application, logcat style: "[app] I/tag: message", one line per message line
			 * @param priority_ Android priority letter: V, D, I, W, E or F
			 * @param tag_ log tag
			 * @param msg_ message
			 */
			void app(char priority_, std::string_view tag_, std::string_view msg_);

		private:
			friend class Singleton<Logger>;
			Logger();
			~Logger() override;

			std::string getTime() const;
			std::string getThreadName() const;
			void writeLine(std::string_view marker_, uint32_t color_, bool bold_, bool important_, std::string_view msg_, std::string_view coloredMsg_);

			bool _console = true;
			bool _colors = false;
			bool _time = false;
			bool _logThreadName = false;
			static inline std::atomic<LogLevel> _level{LogLevel::INFO};
			static inline std::atomic<LogLevel> _appLevel{LogLevel::INFO};
			std::ofstream _file;

			/** map thread id, thread name (for threads registered from another thread) */
			std::map<std::thread::id, std::string> _threads;
			std::mutex _threadsMutex;
			/** thread safe logging */
			std::mutex _mutex;
	};

	/** @brief Get the global logger instance
	 * @return Reference to the global Logger instance
	 */
	inline Logger &getLogger() {
		return Logger::getInstance();
	}
}  // namespace sandvik

#endif
