#include "classbuilder.hpp"

#include "class.hpp"
#include "classloader.hpp"
#include "field.hpp"
#include "frame.hpp"
#include "method.hpp"
#include "object.hpp"

using namespace sandvik;

ClassBuilder::ClassBuilder(ClassLoader& classLoader, const std::string& packageName, const std::string& className) : _classLoader(classLoader) {
	_class = std::make_unique<Class>(classLoader, packageName, className);
}

void ClassBuilder::setSuperClass(const std::string& superClassName_) {
	_class->_superClassname = superClassName_;
}

void ClassBuilder::setInterface() {
	_class->_isInterface = true;
}

void ClassBuilder::addVirtualMethod(const std::string& name_, const std::string& signature_, uint64_t flags_,
                                    std::function<void(Frame&, std::vector<std::shared_ptr<Object>>&)> function_) {
	auto method = std::make_unique<Method>(*_class, name_, signature_);
	method->_isVirtual = true;
	method->_accessFlags = flags_;
	method->_function = function_;
	_class->_methods[name_ + signature_] = std::move(method);
}

void ClassBuilder::addMethod(const std::string& name_, const std::string& signature_, uint64_t flags_,
                             std::function<void(Frame&, std::vector<std::shared_ptr<Object>>&)> function_) {
	auto method = std::make_unique<Method>(*_class, name_, signature_);
	method->_isVirtual = false;
	method->_accessFlags = flags_;
	method->_function = function_;
	_class->_methods[name_ + signature_] = std::move(method);
}

void ClassBuilder::addField(const std::string& name_, const std::string& type_, bool isStatic_, std::shared_ptr<Object> value_) {
	_class->_fields[name_] = std::make_unique<Field>(*_class, name_, type_, isStatic_);
	if (isStatic_ && value_) {
		_class->_fields[name_]->setObjectValue(value_);
	}
}

void ClassBuilder::finalize() {
	_classLoader.addClass(std::move(_class));
}