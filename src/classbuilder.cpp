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

#include "classbuilder.hpp"

#include "class.hpp"
#include "classloader.hpp"
#include "field.hpp"
#include "frame.hpp"
#include "method.hpp"
#include "object.hpp"

using namespace sandvik;

ClassBuilder::ClassBuilder(ClassLoader& classLoader, const std::string& packageName, const std::string& className)
    : _classLoader(classLoader), _class(std::make_unique<Class>(classLoader, packageName, className)) {
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