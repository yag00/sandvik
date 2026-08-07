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

#ifndef __RTLD_LOADER_HPP__
#define __RTLD_LOADER_HPP__

#include <cstdint>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace sandvik {
	class Dex;
	/** @brief Runtime loader namespace */
	namespace rtld {
		/** @brief Loads JAR files from the specified runtime path.
		 * @param path_ Path to the JAR file
		 * @param dexs_ Reference to the vector of unique pointers to Dex objects, filled with the loaded Dex files
		 */
		void loadJar(const std::string& path_, std::vector<std::unique_ptr<Dex>>& dexs_);

		/** @brief Resolves the ordered list of jar files in a directory (via order.txt if present, alphabetical otherwise).
		 * @param jarsDir_ Path to the directory containing JAR files
		 * @return Vector of strings containing the resolved JAR file paths
		 */
		std::vector<std::string> resolveJarOrder(const std::string& jarsDir_);

		/** @brief Loads all jars from a directory, in resolved order.
		 * @param jarsDir_ path to directory containing jar files
		 * @param dexs_ reference to vector of unique pointers to Dex objects, filled with
		 * @param loadedJarPaths_ optional out-param filled with the resolved jar paths, in load order */
		void load(const std::string& jarsDir_, std::vector<std::unique_ptr<Dex>>& dexs_, std::vector<std::string>* loadedJarPaths_ = nullptr);

		/** @brief Finds a resource in the specified JAR file.
		 * @param jarPath_ Path to the JAR file
		 * @param name_ Name of the resource to find
		 * @return Optional vector of bytes containing the resource data if found, std::nullopt otherwise
		 */
		std::optional<std::vector<uint8_t>> findResourceInJar(const std::string& jarPath_, const std::string& name_);
	}  // namespace rtld
}  // namespace sandvik
#endif  // __RTLD_LOADER_HPP__