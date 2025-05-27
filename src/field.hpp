#ifndef __FIELD_HPP__
#define __FIELD_HPP__

#include <memory>
#include <string>
#include <vector>

namespace LIEF {
	namespace DEX {
		class Field;
	}
}  // namespace LIEF

namespace sandvik {
	class Class;
	class Object;
	class Field {
		public:
			Field(Class& class_, const std::string& name_, const std::string& type_, bool isStatic_);
			Field(Class& class_, const LIEF::DEX::Field& field_);
			~Field() = default;

			uint32_t getIntValue() const;
			uint64_t getLongValue() const;
			std::string getStringValue() const;
			std::shared_ptr<Object> getObjectValue() const;
			void setIntValue(uint32_t value);
			void setLongValue(uint64_t value);
			void setStringValue(const std::string& value);
			void setObjectValue(std::shared_ptr<Object> value);
			Class& getClass() const;
			std::string getName() const;
			std::string getType() const;
			bool isStatic() const;

		private:
			Class& _class;
			std::string _name;
			std::string _type;
			bool _isStatic;

			uint64_t _value;
			std::string _strValue;
			std::shared_ptr<Object> _obj;
	};
}  // namespace sandvik

#endif  // __FIELD_HPP__