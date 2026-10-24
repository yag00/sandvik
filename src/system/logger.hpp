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
#ifndef SANDVIK_LOG_ACTIVE_LEVEL
#ifdef __debug__
#define SANDVIK_LOG_ACTIVE_LEVEL 0
#else
#define SANDVIK_LOG_ACTIVE_LEVEL 2
#endif
#endif

/** Log a formatted message. The level is checked before the arguments are evaluated and formatted,
 * so a disabled message costs a single branch (and nothing at all below SANDVIK_LOG_ACTIVE_LEVEL).
 */
#define SANDVIK_LOG(level_, ...)                                              \
	do {                                                                      \
		if constexpr (static_cast<int>(level_) >= SANDVIK_LOG_ACTIVE_LEVEL) { \
			if (::sandvik::Logger::isEnabled(level_)) [[unlikely]] {          \
				logger.flog(level_, __VA_ARGS__);                             \
			}                                                                 \
		}                                                                     \
	} while (0)

#define SLOG_VERBOSE(...) SANDVIK_LOG(::sandvik::Logger::LogLevel::VERBOSE, __VA_ARGS__)
#define SLOG_TRACE(...) SANDVIK_LOG(::sandvik::Logger::LogLevel::TRACE, __VA_ARGS__)
#define SLOG_DEBUG(...) SANDVIK_LOG(::sandvik::Logger::LogLevel::DEBUG, __VA_ARGS__)
#define SLOG_INFO(...) SANDVIK_LOG(::sandvik::Logger::LogLevel::INFO, __VA_ARGS__)
#define SLOG_WARN(...) SANDVIK_LOG(::sandvik::Logger::LogLevel::WARNING, __VA_ARGS__)
#define SLOG_OK(...) SANDVIK_LOG(::sandvik::Logger::LogLevel::OK, __VA_ARGS__)
#define SLOG_ERROR(...) SANDVIK_LOG(::sandvik::Logger::LogLevel::ERROR, __VA_ARGS__)

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

			/** formatted log message, use the SLOG_* macros instead: they skip argument evaluation when the level is disabled
			 * @param level_ log level
			 * @param fmt_ format string
			 * @param args_ arguments
			 */
			template <typename... Args>
			void flog(LogLevel level_, fmt::format_string<Args...> fmt_, Args &&...args_) {
				vlog(level_, fmt_.get(), fmt::make_format_args(args_...));
			}
			/** formatted debug log message (prefer SLOG_DEBUG)
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			void fdebug(fmt::format_string<Args...> fmt_str, Args &&...args) {
				if (isEnabled(LogLevel::DEBUG)) {
					vlog(LogLevel::DEBUG, fmt_str.get(), fmt::make_format_args(args...));
				}
			}
			/** formatted info log message (prefer SLOG_INFO)
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			void finfo(fmt::format_string<Args...> fmt_str, Args &&...args) {
				if (isEnabled(LogLevel::INFO)) {
					vlog(LogLevel::INFO, fmt_str.get(), fmt::make_format_args(args...));
				}
			}
			/** formatted warning log message (prefer SLOG_WARN)
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			void fwarning(fmt::format_string<Args...> fmt_str, Args &&...args) {
				if (isEnabled(LogLevel::WARNING)) {
					vlog(LogLevel::WARNING, fmt_str.get(), fmt::make_format_args(args...));
				}
			}
			/** formatted error log message (prefer SLOG_ERROR)
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			void ferror(fmt::format_string<Args...> fmt_str, Args &&...args) {
				if (isEnabled(LogLevel::ERROR)) {
					vlog(LogLevel::ERROR, fmt_str.get(), fmt::make_format_args(args...));
				}
			}
			/** formatted ok log message (prefer SLOG_OK)
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			void fok(fmt::format_string<Args...> fmt_str, Args &&...args) {
				if (isEnabled(LogLevel::OK)) {
					vlog(LogLevel::OK, fmt_str.get(), fmt::make_format_args(args...));
				}
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

		private:
			friend class Singleton<Logger>;
			Logger();
			~Logger() override;

			std::string getTime() const;
			std::string getThreadName() const;
			void writeLine(Kind kind_, std::string_view marker_, uint32_t color_, bool bold_, std::string_view msg_);

			bool _console = true;
			bool _colors = false;
			bool _time = false;
			bool _logThreadName = false;
			static inline std::atomic<LogLevel> _level{LogLevel::INFO};
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
