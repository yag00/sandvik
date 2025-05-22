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

			Method& resolveMethod(uint32_t dex_, uint16_t idx_);
			Method& resolveMethod(uint32_t dex_, uint16_t idx_, std::string& class_, std::string& method_, std::string& sig_);
			Class& resolveClass(uint32_t dex_, uint16_t idx_);
			Class& resolveClass(uint32_t dex_, uint16_t idx_, std::string& classname_);
			Field& resolveField(uint32_t dex_, uint16_t idx_);
			Field& resolveField(uint32_t dex_, uint16_t idx_, std::string& classname, std::string& field);
			std::string resolveString(uint32_t dex_, uint16_t idx_);
			std::vector<std::pair<std::string, uint32_t>> resolveArray(uint32_t dex_, uint16_t idx_);

		private:
			std::vector<std::string> _classpath;
			std::vector<std::unique_ptr<Apk>> _apks;
			std::vector<std::unique_ptr<Dex>> _dexs;
			std::map<std::string, std::unique_ptr<Class>> _classes;
	};
}  // namespace sandvik

#endif