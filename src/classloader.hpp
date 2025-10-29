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
	enum class TYPES;
	/** @brief Class Loader class
	 */
	class ClassLoader {
		public:
			ClassLoader();
			~ClassLoader();

			/** @brief Load runtime classes
			 * @param rt_ path to runtime classes
			 */
			void loadRt(const std::string& rt_);
			/** @brief Load dex file
			 * @param dex_ path to dex
			 */
			void loadDex(const std::string& dex_);
			/** @brief Load apk file
			 * @param apk_ path to apk
			 */
			void loadApk(const std::string& apk_);
			/** @brief Add path to classpath
			 * @param classpath_ path to add
			 */
			void addClassPath(const std::string& classpath_);
			/** @brief Get classpath as a string
			 * @return classpath string
			 */
			std::string getClassPath() const;
			/** @brief Get main activity class name
			 * @return main activity class name
			 */
			std::string getMainActivity() const;
			/** @brief Get main activity class
			 * @return reference to main activity class
			 */
			Class& getMainActivityClass();

			/** @brief Get or load class by name
			 * @param classname_ class name
			 * @return reference to class
			 */
			Class& getOrLoad(const std::string& classname_);

			/** @brief Resolve method by dex and index
			 * @param dex_ dex index
			 * @param idx_ method index
			 * @return reference to method
			 */
			Method& resolveMethod(uint32_t dex_, uint16_t idx_);
			/** @brief Resolve method by dex and index with names
			 * @param dex_ dex index
			 * @param idx_ method index
			 * @param classname_ reference to store class name
			 * @param method_ reference to store method name
			 * @param sig_ reference to store method signature
			 * @return reference to method
			 */
			Method& resolveMethod(uint32_t dex_, uint16_t idx_, std::string& classname_, std::string& method_, std::string& sig_);
			/** @brief Find method names by dex and index
			 * @param dex_ dex index
			 * @param idx_ method index
			 * @param classname_ reference to store class name
			 * @param method_ reference to store method name
			 * @param sig_ reference to store method signature
			 */
			void findMethod(uint32_t dex_, uint16_t idx_, std::string& classname_, std::string& method_, std::string& sig_);
			/** @brief Resolve class by dex and index
			 * @param dex_ dex index
			 * @param idx_ class index
			 * @return reference to class
			 */
			Class& resolveClass(uint32_t dex_, uint16_t idx_);
			/** @brief Resolve class by dex and index with name
			 * @param dex_ dex index
			 * @param idx_ class index
			 * @param classname_ reference to store class name
			 * @return reference to class
			 */
			Class& resolveClass(uint32_t dex_, uint16_t idx_, std::string& classname_);
			/** @brief Resolve field by dex and index
			 * @param dex_ dex index
			 * @param idx_ field index
			 * @return reference to field
			 */
			Field& resolveField(uint32_t dex_, uint16_t idx_);
			/** @brief Resolve field by dex and index with names
			 * @param dex_ dex index
			 * @param idx_ field index
			 * @param classname_ reference to store class name
			 * @param field_ reference to store field name
			 * @return reference to field
			 */
			Field& resolveField(uint32_t dex_, uint16_t idx_, std::string& classname_, std::string& field_);
			/** @brief Resolve type by dex and index
			 * @param dex_ dex index
			 * @param idx_ type index
			 * @param type_ reference to store type enum
			 * @return type as string
			 */
			std::string resolveType(uint32_t dex_, uint16_t idx_, TYPES& type_);
			/** @brief Resolve string by dex and index
			 * @param dex_ dex index
			 * @param idx_ string index
			 * @return resolved string
			 */
			std::string resolveString(uint32_t dex_, uint16_t idx_);
			/** @brief Resolve array type by dex and index
			 * @param dex_ dex index
			 * @param idx_ type index
			 * @return vector of pairs (type as string, array dimension)
			 */
			std::vector<std::pair<std::string, uint32_t>> resolveArray(uint32_t dex_, uint16_t idx_);
			/** @brief Get dex index
			 * @param dex_ reference to dex
			 * @return dex index
			 */
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