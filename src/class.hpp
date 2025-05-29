#ifndef __CLASS_HPP__
#define __CLASS_HPP__

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace LIEF {
	namespace DEX {
		class Class;
	}
}  // namespace LIEF

namespace sandvik {
	class ClassLoader;
	class Object;
	class Method;
	class Field;
	class Class {
		public:
			Class(ClassLoader& classloader_, const std::string& packagename_, const std::string& fullname_);
			Class(ClassLoader& classloader_, const uint32_t dexIdx_, const LIEF::DEX::Class& class_);
			virtual ~Class() = default;

			void debug() const;

			bool isStaticInitialized();
			void setStaticInitialized();

			uint32_t getDexIdx() const;
			std::string getName() const;
			std::string getFullname() const;

			Method& getMethod(const std::string& name_, const std::string& descriptor_);
			Method& getMethod(uint32_t idx_);

			Field& getField(const std::string& name_);
			Field& getField(uint32_t idx_);
			std::vector<std::string> getFieldList();

			bool isInstanceOf(const std::string& class_) const;
			bool isInstanceOf(std::shared_ptr<Object>& class_) const;
			bool isExternal() const;
			bool isAbstract() const;
			bool isInterface() const;
			bool hasSuperClass() const;

			Class& getSuperClass() const;
			std::string getSuperClassname() const;

		private:
			ClassLoader& _classloader;
			bool _isStaticInitialized;

			std::string _packagename;
			std::string _fullname;
			std::string _name;
			uint32_t _dexIdx;

			bool _isInterface;
			bool _isAbstract;
			bool _hasSuperClass;
			std::string _superClassname;

			std::map<std::string, std::unique_ptr<Method>> _methods;
			std::map<std::string, std::unique_ptr<Field>> _fields;
			friend class ClassBuilder;
	};
}  // namespace sandvik

#endif  // __CLASS_HPP__