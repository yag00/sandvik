#include "field.hpp"

#include <LIEF/DEX/Class.hpp>
#include <LIEF/DEX/Field.hpp>
#include <LIEF/DEX/Type.hpp>

#include "class.hpp"
#include "object.hpp"
#include "utils.hpp"

using namespace sandvik;

Field::Field(Class& class_, const std::string& name_, const std::string& type_, bool isStatic_)
    : _class(class_), _name(name_), _type(type_), _isStatic(isStatic_), _value(0), _strValue(""), _obj(Object::makeNull()) {
}
Field::Field(Class& class_, const LIEF::DEX::Field& field_)
    : _class(class_),
      _name(field_.name()),
      _type(get_type_descriptor(*field_.type())),
      _isStatic(field_.has(LIEF::DEX::ACC_STATIC)),
      _value(0),
      _strValue(""),
      _obj(Object::makeNull()) {
}

Class& Field::getClass() const {
	return _class;
}

std::string Field::getName() const {
	return _name;
}

std::string Field::getType() const {
	return _type;
}

bool Field::isStatic() const {
	return _isStatic;
}

uint32_t Field::getIntValue() const {
	return static_cast<uint32_t>(_value);
}

uint64_t Field::getLongValue() const {
	return _value;
}
std::string Field::getStringValue() const {
	return _strValue;
}

std::shared_ptr<Object> Field::getObjectValue() const {
	return _obj;
}

void Field::setIntValue(uint32_t value) {
	_value = value;
}
void Field::setLongValue(uint64_t value) {
	_value = value;
}

void Field::setStringValue(const std::string& value) {
	_strValue = value;
}

void Field::setObjectValue(std::shared_ptr<Object> value) {
	_obj = value;
}
