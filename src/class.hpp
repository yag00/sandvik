#ifndef __CLASS_HPP__
#define __CLASS_HPP__

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "field.hpp"
#include "method.hpp"

namespace LIEF {
	namespace DEX {
		class Class;
	}
}  // namespace LIEF

namespace sandvik {
	class Object;
	class Method;
	class Field;
	class Class {
		public:
			explicit Class(const uint32_t dexIdx_, const LIEF::DEX::Class& class_);
			~Class() = default;
			Class(const Class& other);

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

			bool inheritsFrom(Class& class_) const;
			bool isInstanceOf(std::shared_ptr<Object>& class_) const;
			bool isExternal() const;
			bool isAbstract() const;
			bool isPrimitive() const;
			bool isInterface() const;
			bool hasSuperClass() const;

		private:
			const uint32_t _dexIdx;
			const LIEF::DEX::Class& _class;
			bool _isStaticInitialized;
			std::map<std::string, std::unique_ptr<Method>> _methods;
			std::map<std::string, std::unique_ptr<Field>> _fields;
	};
}  // namespace sandvik

#endif  // __CLASS_HPP__