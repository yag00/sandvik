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

#ifndef __GETPROP_LOADER_HPP__
#define __GETPROP_LOADER_HPP__

#include <cstddef>
#include <mutex>
#include <string>
#include <unordered_map>

#include "system/singleton.hpp"

namespace sandvik {
	/**
	 * @brief Singleton loader for Android getprop output.
	 *
	 * Supported line formats:
	 * - [key]: [value]
	 * - key=value
	 */
	class Getprop : public Singleton<Getprop> {
			friend class Singleton<Getprop>;

		public:
			/**
			 * @brief Parse and load properties from raw getprop text output.
			 * @param output_ Full text returned by an android getprop command.
			 * @return Number of properties stored.
			 */
			size_t loadFromText(const std::string& output_);

			/**
			 * @brief Parse and load properties from a file containing getprop output.
			 * @param path_ Path to the input file.
			 * @return Number of properties stored.
			 */
			size_t loadFromFile(const std::string& path_);

			/**
			 * @brief Retrieve a property value by key.
			 * @param key_ Property key.
			 * @param defaultValue_ Returned when key does not exist.
			 * @return Property value or defaultValue_.
			 */
			std::string get(const std::string& key_, const std::string& defaultValue_ = "") const;

			/**
			 * @brief Check if a property key exists.
			 * @param key_ Property key.
			 * @return true when key exists.
			 */
			bool has(const std::string& key_) const;

			/**
			 * @brief Remove all loaded properties.
			 */
			void clear();

		private:
			Getprop() = default;
			~Getprop() = default;

			static bool parseLine(const std::string& line_, std::string& keyOut_, std::string& valueOut_);
			static std::string trim(const std::string& value_);

			mutable std::mutex _mutex;
			std::unordered_map<std::string, std::string> _properties;
	};
}  // namespace sandvik

#endif  // __GETPROP_LOADER_HPP__
