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

#ifndef __EXCEPTIONS_HPP__
#define __EXCEPTIONS_HPP__

#include <exception>
#include <string>

namespace sandvik {
	/** @brief StackOverflowError exception class */
	/** @brief NullPointerException exception class */
	class NullPointerException : public std::exception {
		public:
			/** constructor */
			NullPointerException(const std::string& message = "");
			/** destructor */
			~NullPointerException() noexcept;
			/** get exception message
			 * @return exception message
			 */
			const char* what() const noexcept override;

		private:
			std::string _message;
	};
	class StackOverflowError : public std::exception {
		public:
			/** constructor */
			StackOverflowError();
			/** destructor */
			~StackOverflowError() noexcept;
			/** get exception message
			 * @return exception message
			 */
			const char* what() const noexcept override;

		private:
	};
	/** @brief StackOverflowError exception class */
	class OutOfMemoryError : public std::exception {
		public:
			/** constructor */
			OutOfMemoryError();
			/** destructor */
			~OutOfMemoryError() noexcept;
			/** get exception message
			 * @return exception message
			 */
			const char* what() const noexcept override;

		private:
	};

}  // namespace sandvik

#endif