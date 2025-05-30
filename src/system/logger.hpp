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

#include <fstream>
#include <string>

#define logger ::sandvik::Logger::getInstance()

namespace sandvik {
	/** @brief Logger class
	 * @todo : add fmt log method when std::format will be supported.
	 * We do not want to directly expose fmt library to user.
	 */
	class Logger {
		public:
			/** enum for log level */
			enum class LogLevel { NONE, ERROR, OK, WARNING, INFO, DEBUG };

			/** get logger instance
			 * @return the logger
			 */
			static Logger &getInstance() {
				static Logger instance;
				return instance;
			}

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
			 * @param level log level
			 */
			void setLevel(LogLevel level);
			/** return true if log message is prefixed with time
			 * @return if log message is prefixed with time
			 */
			bool isDisplayingTime() const;
			/** prefix log message with time
			 * @param enable
			 */
			void displayTime(bool enable);

			/** log info message
			 * @param msg message
			 */
			void info(const std::string &msg);
			/** log debug message
			 * @param msg message
			 */
			void debug(const std::string &msg);
			/** log warning message
			 * @param msg message
			 */
			void warning(const std::string &msg);
			/** log error message
			 * @param msg message
			 */
			void error(const std::string &msg);
			/** log ok message
			 * @param msg message
			 */
			void ok(const std::string &msg);

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

		private:
			Logger();
			~Logger();

			std::string getTime() const;
			void log(LogLevel level, const std::string &msg);

			bool _stdout;
			bool _time;
			LogLevel _level;
			std::ofstream _file;
	};
}  // namespace sandvik

#endif