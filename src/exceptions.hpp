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