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

#ifndef __CLASS_LOADER_HPP__
#define __CLASS_LOADER_HPP__

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace sandvik {
	class Class;
	class Method;
	class Field;
	class Apk;
	class Dex;
	/** @brief Class Loader class
	 */
	class ClassLoader {
		public:
			ClassLoader();
			~ClassLoader();

			void loadRt(const std::string& rt_);
			void loadDex(const std::string& dex_);
			void loadApk(const std::string& apk_);
			void addClassPath(const std::string& classpath_);
			std::string getClassPath() const;
			std::string getMainActivity() const;
			Class& getMainActivityClass();

			Class& getOrLoad(const std::string& classname_);

			Method& resolveMethod(uint32_t dex_, uint16_t idx_);
			Method& resolveMethod(uint32_t dex_, uint16_t idx_, std::string& class_, std::string& method_, std::string& sig_);
			void findMethod(uint32_t dex_, uint16_t idx_, std::string& class_, std::string& method_, std::string& sig_);
			Class& resolveClass(uint32_t dex_, uint16_t idx_);
			Class& resolveClass(uint32_t dex_, uint16_t idx_, std::string& classname_);
			Field& resolveField(uint32_t dex_, uint16_t idx_);
			Field& resolveField(uint32_t dex_, uint16_t idx_, std::string& classname, std::string& field);
			std::string resolveString(uint32_t dex_, uint16_t idx_);
			std::vector<std::pair<std::string, uint32_t>> resolveArray(uint32_t dex_, uint16_t idx_);

			uint64_t getDexIndex(const Dex& dex_) const;

		private:
			friend class ClassBuilder;
			void addClass(std::unique_ptr<Class> class_);

			std::vector<std::string> _classpath;
			std::vector<std::unique_ptr<Apk>> _apks;
			std::vector<std::unique_ptr<Dex>> _dexs;
			std::map<std::string, std::unique_ptr<Class>> _classes;
	};
}  // namespace sandvik

#endif