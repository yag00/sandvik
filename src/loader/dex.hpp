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

#ifndef __DEX_LOADER_HPP__
#define __DEX_LOADER_HPP__

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace LIEF {
	namespace DEX {
		class File;
	}  // namespace DEX
}  // namespace LIEF

namespace sandvik {
	class ClassLoader;
	class Class;
	enum class TYPES;
	/**
	 * @brief Dex file loader.
	 *
	 * The Dex class is responsible for loading and parsing DEX (Dalvik Executable) files
	 */
	class Dex {
		public:
			/** @brief Exception class for Dex loading errors. */
			class DexLoaderException : public std::runtime_error {
				public:
					using std::runtime_error::runtime_error;
			};

			/** @brief Load a Dex file from a path.
			 * @param path_ Path to the DEX file
			 */
			explicit Dex(const std::string& path_);
			/** @brief Load a Dex file from a memory buffer.
			 * @param buffer Reference to the vector containing the DEX file data
			 * @param path_ Path to the DEX file (for debugging purposes)
			 */
			explicit Dex(std::vector<uint8_t>& buffer, const std::string& path_);
			Dex();
			~Dex();

			// Prevent copying
			Dex(const Dex&) = delete;
			Dex& operator=(const Dex&) = delete;

			/** @brief Gets the path to the DEX file.
			 * @return Path to the DEX file
			 */
			std::string getPath() const;

			/** @brief Loads the DEX file from the specified path.
			 * @param path Path to the DEX file
			 */
			void load(const std::string& path);
			/** @brief Loads the DEX file from a memory buffer.
			 * @param buffer Reference to the vector containing the DEX file data
			 */
			void load(std::vector<uint8_t>& buffer);

			/** @brief Checks if the DEX file is loaded.
			 * @return true if the DEX file is loaded, false otherwise
			 */
			bool is_loaded() const noexcept;
			/** @brief Retrieves all class names defined in the DEX file.
			 * @return Vector of class names
			 */
			std::vector<std::string> getClassNames() const;
			/** @brief Finds and returns a Class object by its name.
			 * @param classloader_ Reference to the ClassLoader
			 * @param name Name of the class to find
			 * @return Unique pointer to the Class object if found, nullptr otherwise
			 */
			std::unique_ptr<Class> findClass(ClassLoader& classloader_, const std::string& name) const;
			/** @brief Resolves a method by its index.
			 * @param idx Index of the method to resolve
			 * @param class_ class name
			 * @param method_ method name
			 * @param sig_ method signature
			 */
			void resolveMethod(uint16_t idx, std::string& class_, std::string& method_, std::string& sig_) const;
			/** @brief Resolves a class by its index.
			 * @param idx Index of the class to resolve
			 * @param class_ class name
			 */
			void resolveClass(uint16_t idx, std::string& class_) const;
			/** @brief Resolves a field by its index.
			 * @param idx Index of the field to resolve
			 * @param class_ class name
			 * @param field_ field name
			 */
			void resolveField(uint16_t idx, std::string& class_, std::string& field_) const;
			/** @brief Resolves a type by its index.
			 * @param idx Index of the type to resolve
			 * @param type_ Reference to the TYPES enum to store the resolved type
			 * @return Resolved type as a string
			 */
			std::string resolveType(uint16_t idx, TYPES& type_);
			/** @brief Resolves a string by its index.
			 * @param idx Index of the string to resolve
			 * @return Resolved string
			 */
			std::string resolveString(uint16_t idx);
			/** @brief Resolves an array type by its index.
			 * @param idx Index of the array type to resolve
			 * @return the resolved array type information (descriptor, dimension)
			 */
			std::vector<std::pair<std::string, uint32_t>> resolveArray(uint16_t idx);

		private:
			std::string _path;
			std::unique_ptr<const LIEF::DEX::File> _dex;
	};
}  // namespace sandvik
#endif  // __DEX_LOADER_HPP__