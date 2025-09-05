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
			explicit SharedLibrary(const std::string& name_);
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
