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

#include <fmt/format.h>

#include <exception>
#include <string>

namespace sandvik {
	/**
	 * @class VmException
	 * @brief Exception class representing a VM exception.
	 */
	class VmException : public std::exception {
		public:
			/** exception constructor
			 * @param message_ Exception message. */
			explicit VmException(const std::string& message_ = "");
			/** formatted exception constructor
			 * @param fmt_str format string
			 * @param args arguments
			 */
			template <typename... Args>
			explicit VmException(fmt::format_string<Args...> fmt_str, Args&&... args) {
				_message = fmt::format(fmt_str, std::forward<Args>(args)...);
			}
			~VmException() noexcept override = default;
			/** get exception message
			 * @return exception message
			 */
			const char* what() const noexcept override;

		private:
			std::string _message;
	};
	/**
	 * @class JavaException
	 * @brief Exception class representing a Java exception.
	 */
	class JavaException : public std::exception {
		public:
			/** exception constructor
			 * @param type_ exception class type
			 * @param message_ Exception message. */
			explicit JavaException(const std::string& type_, const std::string& message_ = "");
			~JavaException() noexcept override = default;
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
			/** constructor
			 * @param message Exception message. */
			explicit ArithmeticException(const std::string& message = "") : JavaException("java.lang.ArithmeticException", message) {
			}
			~ArithmeticException() noexcept override = default;
	};
	/** @brief NullPointerException exception class */
	class NullPointerException : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit NullPointerException(const std::string& message = "") : JavaException("java.lang.NullPointerException", message) {
			}
			/** destructor */
			~NullPointerException() noexcept override = default;
	};
	/** @brief ClassCastException exception class */
	class ClassCastException : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit ClassCastException(const std::string& message = "") : JavaException("java.lang.ClassCastException", message) {
			}
			/** destructor */
			~ClassCastException() noexcept override = default;
	};
	/** @brief NoClassDefFoundError exception class */
	class NoClassDefFoundError : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit NoClassDefFoundError(const std::string& message = "") : JavaException("java.lang.NoClassDefFoundError", message) {
			}
			~NoClassDefFoundError() noexcept override = default;
	};

	/** @brief InstantiationException exception class */
	class InstantiationException : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit InstantiationException(const std::string& message = "") : JavaException("java.lang.InstantiationException", message) {
			}
			~InstantiationException() noexcept override = default;
	};

	/** @brief IllegalAccessError exception class */
	class IllegalAccessError : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit IllegalAccessError(const std::string& message = "") : JavaException("java.lang.IllegalAccessError", message) {
			}
			~IllegalAccessError() noexcept override = default;
	};

	/** @brief IndexOutOfBoundsException exception class */
	class IndexOutOfBoundsException : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit IndexOutOfBoundsException(const std::string& message = "") : JavaException("java.lang.IndexOutOfBoundsException", message) {
			}
			~IndexOutOfBoundsException() noexcept override = default;
	};

	/** @brief IllegalAccessException exception class */
	class IllegalAccessException : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit IllegalAccessException(const std::string& message = "") : JavaException("java.lang.IllegalAccessException", message) {
			}
			~IllegalAccessException() noexcept override = default;
	};
	/** @brief IllegalArgumentException exception class */
	class IllegalArgumentException : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit IllegalArgumentException(const std::string& message = "") : JavaException("java.lang.IllegalArgumentException", message) {
			}
			~IllegalArgumentException() noexcept override = default;
	};

	/** @brief ExceptionInInitializerError exception class */
	class ExceptionInInitializerError : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit ExceptionInInitializerError(const std::string& message = "") : JavaException("java.lang.ExceptionInInitializerError", message) {
			}
			~ExceptionInInitializerError() noexcept override = default;
	};
	/** @brief ArrayIndexOutOfBoundsException exception class */
	class ArrayIndexOutOfBoundsException : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit ArrayIndexOutOfBoundsException(const std::string& message = "") : JavaException("java.lang.ArrayIndexOutOfBoundsException", message) {
			}
			~ArrayIndexOutOfBoundsException() noexcept override = default;
	};
	/** @brief ArrayStoreException exception class */
	class ArrayStoreException : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit ArrayStoreException(const std::string& message = "") : JavaException("java.lang.ArrayStoreException", message) {
			}
			~ArrayStoreException() noexcept override = default;
	};
	/** @brief NegativeArraySizeException exception class */
	class NegativeArraySizeException : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit NegativeArraySizeException(const std::string& message = "") : JavaException("java.lang.NegativeArraySizeException", message) {
			}
			~NegativeArraySizeException() noexcept override = default;
	};
	/** @brief VerifyError exception class */
	class VerifyError : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit VerifyError(const std::string& message = "") : JavaException("java.lang.VerifyError", message) {
			}
			~VerifyError() noexcept override = default;
	};
	/** @brief NumberFormatException exception class */
	class NumberFormatException : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit NumberFormatException(const std::string& message = "") : JavaException("java.lang.NumberFormatException", message) {
			}
			~NumberFormatException() noexcept override = default;
	};
	/** @brief StringIndexOutOfBoundsException exception class */
	class StringIndexOutOfBoundsException : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit StringIndexOutOfBoundsException(const std::string& message = "") : JavaException("java.lang.StringIndexOutOfBoundsException", message) {
			}
			~StringIndexOutOfBoundsException() noexcept override = default;
	};
	/** @brief NoSuchMethodError exception class */
	class NoSuchMethodError : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit NoSuchMethodError(const std::string& message = "") : JavaException("java.lang.NoSuchMethodError", message) {
			}
			~NoSuchMethodError() noexcept override = default;
	};
	/** @brief NoSuchFieldException exception class */
	class NoSuchFieldException : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit NoSuchFieldException(const std::string& message = "") : JavaException("java.lang.NoSuchFieldException", message) {
			}
			~NoSuchFieldException() noexcept override = default;
	};
	/** @brief NoSuchMethodException exception class */
	class NoSuchMethodException : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit NoSuchMethodException(const std::string& message = "") : JavaException("java.lang.NoSuchMethodException", message) {
			}
			~NoSuchMethodException() noexcept override = default;
	};
	/** @brief OutOfMemoryError exception class */
	class OutOfMemoryError : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit OutOfMemoryError(const std::string& message = "") : JavaException("java.lang.OutOfMemoryError", message) {
			}
			~OutOfMemoryError() noexcept override = default;
	};
	/** @brief CloneNotSupportedException exception class */
	class CloneNotSupportedException : public JavaException {
		public:
			/** constructor
			 * @param message Exception message. */
			explicit CloneNotSupportedException(const std::string& message = "") : JavaException("java.lang.CloneNotSupportedException", message) {
			}
			~CloneNotSupportedException() noexcept override = default;
	};
}  // namespace sandvik

#endif