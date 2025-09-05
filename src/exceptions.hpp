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
	/**
	 * @class JavaException
	 * @brief Exception class representing a Java exception.
	 */
	class JavaException : public std::exception {
		public:
			/** constructor */
			explicit JavaException(const std::string& type_, const std::string& message_ = "");
			/** destructor */
			~JavaException() noexcept = default;
			/** get exception message
			 * @return exception message
			 */
			const char* what() const noexcept override;
			/** Retrieves the java exception class type.
			 * @return A string containing the exception class type.
			 */
			std::string getExceptionType() const;
			/** Retrieves the exception message.
			 * @return A string containing the exception message.
			 */
			std::string getMessage() const;

		private:
			std::string _type;
			std::string _message;
	};
	/** @brief ArithmeticException exception class */
	class ArithmeticException : public JavaException {
		public:
			/** constructor */
			ArithmeticException(const std::string& message = "") : JavaException("java.lang.ArithmeticException", message) {
			}
			/** destructor */
			~ArithmeticException() noexcept override = default;
	};
	/** @brief NullPointerException exception class */
	class NullPointerException : public JavaException {
		public:
			/** constructor */
			NullPointerException(const std::string& message = "") : JavaException("java.lang.NullPointerException", message) {
			}
			/** destructor */
			~NullPointerException() noexcept override = default;
	};
	/** @brief ClassCastException exception class */
	class ClassCastException : public JavaException {
		public:
			/** constructor */
			ClassCastException(const std::string& message = "") : JavaException("java.lang.ClassCastException", message) {
			}
			/** destructor */
			~ClassCastException() noexcept override = default;
	};
	/** @brief NoClassDefFoundError exception class */
	class NoClassDefFoundError : public JavaException {
		public:
			NoClassDefFoundError(const std::string& message = "") : JavaException("java.lang.NoClassDefFoundError", message) {
			}
			~NoClassDefFoundError() noexcept override = default;
	};

	/** @brief InstantiationException exception class */
	class InstantiationException : public JavaException {
		public:
			InstantiationException(const std::string& message = "") : JavaException("java.lang.InstantiationException", message) {
			}
			~InstantiationException() noexcept override = default;
	};

	/** @brief IllegalAccessError exception class */
	class IllegalAccessError : public JavaException {
		public:
			IllegalAccessError(const std::string& message = "") : JavaException("java.lang.IllegalAccessError", message) {
			}
			~IllegalAccessError() noexcept override = default;
	};

	/** @brief IllegalAccessException exception class */
	class IllegalAccessException : public JavaException {
		public:
			IllegalAccessException(const std::string& message = "") : JavaException("java.lang.IllegalAccessException", message) {
			}
			~IllegalAccessException() noexcept override = default;
	};

	/** @brief ExceptionInInitializerError exception class */
	class ExceptionInInitializerError : public JavaException {
		public:
			ExceptionInInitializerError(const std::string& message = "") : JavaException("java.lang.ExceptionInInitializerError", message) {
			}
			~ExceptionInInitializerError() noexcept override = default;
	};
	/** @brief ArrayIndexOutOfBoundsException exception class */
	class ArrayIndexOutOfBoundsException : public JavaException {
		public:
			ArrayIndexOutOfBoundsException(const std::string& message = "") : JavaException("java.lang.ArrayIndexOutOfBoundsException", message) {
			}
			~ArrayIndexOutOfBoundsException() noexcept override = default;
	};
	/** @brief ArrayStoreException exception class */
	class ArrayStoreException : public JavaException {
		public:
			ArrayStoreException(const std::string& message = "") : JavaException("java.lang.ArrayStoreException", message) {
			}
			~ArrayStoreException() noexcept override = default;
	};
	/** @brief NegativeArraySizeException exception class */
	class NegativeArraySizeException : public JavaException {
		public:
			NegativeArraySizeException(const std::string& message = "") : JavaException("java.lang.NegativeArraySizeException", message) {
			}
			~NegativeArraySizeException() noexcept override = default;
	};
	/** @brief VerifyError exception class */
	class VerifyError : public JavaException {
		public:
			VerifyError(const std::string& message = "") : JavaException("java.lang.VerifyError", message) {
			}
			~VerifyError() noexcept override = default;
	};
}  // namespace sandvik

#endif