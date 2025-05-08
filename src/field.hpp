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
			Field(Class& class_, const LIEF::DEX::Field& field_);
			Field(const Field& other);
			~Field() = default;

			uint32_t getIntValue() const;
			uint64_t getLongValue() const;
			std::string getStringValue() const;
			std::shared_ptr<Object> getObjectValue() const;
			void setIntValue(uint32_t value);
			void setLongValue(uint64_t value);
			void setStringValue(const std::string& value);
			void setObjectValue(std::shared_ptr<Object> value);

			std::string getName() const;
			Class& getClass() const;

			std::string getType() const;
			bool hasClass() const;
			bool isStatic() const;

		private:
			Class& _class;
			const LIEF::DEX::Field& _field;

			uint64_t _value;
			std::string _strValue;
			std::shared_ptr<Object> _obj;
	};
}  // namespace sandvik

#endif  // __OBJECT_HPP__