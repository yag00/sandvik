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

#include <stdarg.h>
#include <stdint.h>

#include <fmt/format.h>

#include <fstream>
#include <map>
#include <mutex>
#include <string>
#include <system/singleton.hpp>
#include <thread>

#define logger ::sandvik::getLogger()

namespace sandvik {
	/** @brief Logger class
	 */
	class Logger : public Singleton<Logger> {
		public:
			/** enum for log level */
			enum class LogLevel { NONE, ERROR, OK, WARNING, INFO, DEBUG };

			/** add thread to logger : log messages from this thread will be prefixed with thread name
			 * @param tid_ thread id
			 * @param name_ thread name
			 */
			void addThread(std::thread::id tid_, const std::string &name_);
			/** remove thread from logger : log messages from this thread won't be prefixed with thread name
			 * @param tid_ thread id
			 */
			void removeThread(std::thread::id tid_);

			/** log to file
			 * @param filename_ filename for log
			 */
			void logToFile(const std::string &filename_);
			/** enable/disable log output to console
			 * @param enable_ enable/disable log on console
			 */
			void logToConsole(bool enable_);

			/** get current log level
			 * @return log level
			 */
			LogLevel getLevel() const;
			/** set log level
			 * @param level_ log level
			 */
			void setLevel(LogLevel level_);
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

			/** printf like log info message
			 * @param msg_ message
			 */
			void info(const char *msg_, ...);
			/** printf like log debug message
			 * @param msg_ printf like format string
			 */
			void debug(const char *msg_, ...);
			/** printf like log warning message
			 * @param msg_ printf like format string
			 */
			void warning(const char *msg_, ...);
			/** printf like log error message
			 * @param msg_ printf like format string
			 */
			void error(const char *msg_, ...);
			/** printf like log ok message
			 * @param msg_ printf like format string
			 */
			void ok(const char *msg_, ...);

			/** printf like log info message with custom color and marker
			 * @param color_ 24 bit hex color
			 * @param marker_ marker character
			 * @param msg_ printf like format string
			 */
			void color(uint32_t color_, char marker_, const char *msg_, ...);
			/** log info message with custom color and marker
			 * @param color_ 24 bit hex color
			 * @param marker_ marker character
			 * @param msg_ message
			 */
			void color(uint32_t color_, char marker_, const std::string &msg_);

			/** formatted debug log message
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			void fdebug(fmt::format_string<Args...> fmt_str, Args &&...args) {
				auto formatted = fmt::format(fmt_str, std::forward<Args>(args)...);
				debug(formatted);
			}
			/** formatted info log message
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			void finfo(fmt::format_string<Args...> fmt_str, Args &&...args) {
				auto formatted = fmt::format(fmt_str, std::forward<Args>(args)...);
				info(formatted);
			}
			/** formatted warning log message
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			void fwarning(fmt::format_string<Args...> fmt_str, Args &&...args) {
				auto formatted = fmt::format(fmt_str, std::forward<Args>(args)...);
				warning(formatted);
			}
			/** formatted error log message
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			void ferror(fmt::format_string<Args...> fmt_str, Args &&...args) {
				auto formatted = fmt::format(fmt_str, std::forward<Args>(args)...);
				error(formatted);
			}
			/** formatted ok log message
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			void fok(fmt::format_string<Args...> fmt_str, Args &&...args) {
				auto formatted = fmt::format(fmt_str, std::forward<Args>(args)...);
				ok(formatted);
			}

		private:
			friend class Singleton<Logger>;
			Logger();
			~Logger() override;

			std::string getTime() const;
			void log(LogLevel level, const std::string &msg);

			bool _stdout = true;
			bool _time = false;
			LogLevel _level = LogLevel::INFO;
			std::ofstream _file;

			/** map thread id, thread name */
			std::map<std::thread::id, std::string> _threads;
			/** map thread name, output properties <log to console, log to file> */
			std::map<std::string, std::pair<bool, bool>, std::less<>> _threadLogOutputs;
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
