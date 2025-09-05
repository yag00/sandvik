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

#ifndef __APK_LOADER_HPP__
#define __APK_LOADER_HPP__

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace sandvik {
	class ClassLoader;
	class Class;
	class Dex;
	class ZipReader;
	class Apk {
		public:
			Apk(const std::string& path_, Dex& classes_dex_);
			~Apk();

			// Prevent copying
			Apk(const Apk&) = delete;
			Apk& operator=(const Apk&) = delete;

			// Allow moving
			Apk(Apk&&) noexcept;
			Apk& operator=(Apk&&) noexcept;

			std::string getPath() const;

			void load(const std::string& path);

			std::vector<std::string> getClassNames() const;
			std::string getMainActivity() const;
			std::unique_ptr<Class> findClass(ClassLoader& classloader_, const std::string& name) const;

		protected:
			std::string findMainActivity() const;

		private:
			std::string _path;
			Dex& _classes_dex;
			std::unique_ptr<ZipReader> _zipReader;
			std::string _manifest;
			std::string _mainActivity;
	};
}  // namespace sandvik
#endif  // __APK_LOADER_HPP__