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

			void loadDex(const std::string& dex_);
			void loadApk(const std::string& apk_);
			void addClassPath(const std::string& classpath_);
			std::string getClassPath() const;
			std::string getMainActivity() const;
			Class& getMainActivityClass();

			Class& getOrLoad(const std::string& classname_);

			Method& resolveMethod(uint16_t idx);
			Method& resolveMethod(uint16_t idx, std::string& class_, std::string& method_, std::string& sig_);
			Class& resolveClass(uint16_t idx);
			Class& resolveClass(uint16_t idx, std::string& classname_);
			Field& resolveField(uint16_t idx);
			std::string resolveString(uint16_t idx);
			std::vector<std::pair<std::string, uint32_t>> resolveArray(uint16_t idx);

		private:
			std::vector<std::string> _classpath;
			std::vector<std::unique_ptr<Apk>> _apks;
			std::vector<std::unique_ptr<Dex>> _dexs;
			std::map<std::string, std::unique_ptr<Class>> _classes;
	};
}  // namespace sandvik

#endif