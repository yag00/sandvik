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
	class Dex {
		public:
			// Custom exception class
			class DexLoaderException : public std::runtime_error {
				public:
					using std::runtime_error::runtime_error;
			};

			explicit Dex(const std::string& path_);
			Dex(std::vector<uint8_t>& buffer, const std::string& name = "");
			Dex();
			~Dex();

			// Prevent copying
			Dex(const Dex&) = delete;
			Dex& operator=(const Dex&) = delete;

			std::string getPath() const;

			void load(const std::string& path);
			void load(std::vector<uint8_t>& buffer, const std::string& name = "");

			bool is_loaded() const noexcept;
			std::vector<std::string> getClassNames() const;
			std::unique_ptr<Class> findClass(ClassLoader& classloader_, const std::string& name) const;

			void resolveMethod(uint16_t idx, std::string& class_, std::string& method_, std::string& sig_);
			void resolveClass(uint16_t idx, std::string& class_);
			void resolveField(uint16_t idx, std::string& class_, std::string& field_);
			std::string resolveType(uint16_t idx, TYPES& type_);
			std::string resolveString(uint16_t idx);
			std::vector<std::pair<std::string, uint32_t>> resolveArray(uint16_t idx);

		private:
			std::string _path;
			std::unique_ptr<const LIEF::DEX::File> _dex;
	};
}  // namespace sandvik
#endif  // __DEX_LOADER_HPP__