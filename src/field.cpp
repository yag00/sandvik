#include "field.hpp"

#include <LIEF/DEX/Class.hpp>
#include <LIEF/DEX/Field.hpp>
#include <LIEF/DEX/Type.hpp>

#include "class.hpp"
#include "object.hpp"
#include "utils.hpp"

using namespace sandvik;

Field::Field(Class& class_, const LIEF::DEX::Field& field_) : _class(class_), _field(field_), _obj(Object::makeNull()) {
}

Field::Field(const Field& other) : _class(other._class), _field(other._field) {
}

std::string Field::getName() const {
	return _field.name();
}

Class& Field::getClass() const {
	return _class;
}

bool Field::hasClass() const {
	return _field.has_class();
}
bool Field::isStatic() const {
	return _field.has(LIEF::DEX::ACC_STATIC);
}

std::string Field::getType() const {
	return get_type_descriptor(*_field.type());
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
