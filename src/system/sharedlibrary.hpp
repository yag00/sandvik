#ifndef __SYSTEM_SHAREDLIBRARY_HPP__
#define __SYSTEM_SHAREDLIBRARY_HPP__

#include <string>

namespace sandvik {
	/** @class SharedLibrary
	 *  @brief Provide access to symbol of shared library.
	 */
	class SharedLibrary {
		public:
			/** constructor
			 * @param name_ name of the shared library */
			SharedLibrary(const std::string& name_);
			~SharedLibrary();

			/** @return the full path of the shared library */
			std::string getFullPath() const;
			/** @return the full path of the shared library */
			static std::string getFullPath(const std::string& name_);

			/** load the library
			 * @throw std::exception */
			void load();
			/** unload the library
			 * @throw std::exception */
			void unload();
			/** return true if library is loaded
			 * @return loaded status */
			bool isLoaded() const;

			/** get a symbol pointer from the library
			 * @return the symbol pointer or null if not available
			 * @throw std::exception */
			void* getAddressOfSymbol(const std::string& name_);

		private:
			std::string _path;
			void* _handle;
	};
}  // namespace sandvik

#endif /* __SYSTEM_SHAREDLIBRARY_HPP__ */
