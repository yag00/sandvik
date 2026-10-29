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

#include "annotation_factory.hpp"

#include <algorithm>
#include <bit>
#include <map>
#include <string>

#include "array.hpp"
#include "class.hpp"
#include "classbuilder.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "frame.hpp"
#include "jthread.hpp"
#include "method.hpp"

using namespace sandvik;

namespace {
	constexpr uint64_t ACC_PUBLIC = 0x0001;
	constexpr const char* IMPLEMENTATION_SUFFIX = "$$SandvikAnnotation";

	std::string returnType(const Method& method_) {
		const auto& signature = method_.getSignature();
		return signature.substr(signature.find(')') + 1);
	}

	/** @brief Class name from a type descriptor ("Ljava/lang/String;" -> "java.lang.String", "I" -> "int") */
	std::string className(const std::string& descriptor_) {
		static const std::map<char, std::string> primitives = {{'Z', "boolean"}, {'B', "byte"}, {'C', "char"},  {'S', "short"},
		                                                       {'I', "int"},     {'J', "long"}, {'F', "float"}, {'D', "double"}};
		if (descriptor_.size() == 1) {
			return primitives.at(descriptor_[0]);
		}
		if (descriptor_[0] == 'L') {
			auto name = descriptor_.substr(1, descriptor_.size() - 2);
			std::replace(name.begin(), name.end(), '/', '.');
			return name;
		}
		return descriptor_;
	}

	ObjectRef toObject(JThread& thread_, const dex::EncodedValue& value_, const std::string& type_) {
		auto& classloader = thread_.getClassLoader();
		switch (value_.kind()) {
			case dex::EncodedValue::Kind::Int:
				return Object::make(static_cast<uint64_t>(value_.asInt()));
			case dex::EncodedValue::Kind::Bool:
				return Object::make(static_cast<uint64_t>(value_.asBool() ? 1 : 0));
			case dex::EncodedValue::Kind::Float:
				if (type_ == "F") {
					return Object::make(static_cast<uint64_t>(std::bit_cast<uint32_t>(static_cast<float>(value_.asFloat()))));
				}
				return Object::make(std::bit_cast<uint64_t>(value_.asFloat()));
			case dex::EncodedValue::Kind::String:
				return Object::make(classloader, value_.asString());
			case dex::EncodedValue::Kind::Type:
				return Object::makeConstClass(classloader, classloader.getOrLoad(value_.asString()));
			case dex::EncodedValue::Kind::Enum: {
				// "package.EnumClass.CONSTANT": the value of the enum constant field
				const auto& name = value_.asString();
				const auto dot = name.rfind('.');
				auto& enumClass = classloader.getOrLoad(name.substr(0, dot));
				thread_.ensureInitialized(enumClass);
				return enumClass.getField(name.substr(dot + 1)).getObjectValue();
			}
			case dex::EncodedValue::Kind::Array: {
				const auto componentType = type_.substr(1);
				const auto& items = value_.asArray();
				auto array = Array::make(classloader.getOrLoad(className(componentType)), static_cast<uint32_t>(items.size()));
				for (size_t i = 0; i < items.size(); ++i) {
					auto item = toObject(thread_, items[i], componentType);
					if (componentType.size() == 1) {
						array->setRawElement(static_cast<uint32_t>(i), static_cast<uint64_t>(item->getLongValue()));
					} else {
						array->setElement(static_cast<uint32_t>(i), item);
					}
				}
				return array;
			}
			case dex::EncodedValue::Kind::Annotation:
				return makeAnnotationInstance(thread_,
				                              dex::Annotation(value_.annotationType(), dex::AnnotationVisibility::RUNTIME, value_.annotationElements()));
			default:
				return Object::makeNull();
		}
	}

	/** @brief Implementation class of an annotation type, synthesized on first use */
	Class& implementationClass(ClassLoader& classloader_, Class& annotationType_) {
		const auto name = annotationType_.getFullname() + IMPLEMENTATION_SUFFIX;
		if (classloader_.isLoaded(name)) {
			return classloader_.getOrLoad(name);
		}
		const auto& typeName = annotationType_.getFullname();
		ClassBuilder builder(classloader_, typeName.substr(0, typeName.rfind('.') == std::string::npos ? 0 : typeName.rfind('.')), name);
		builder.setSuperClass("java.lang.Object");
		builder.addInterface(annotationType_.getFullname());
		for (auto* method : annotationType_.getDeclaredMethods()) {
			if (method->isStatic() || method->getSignature().rfind("()", 0) != 0) {
				continue;
			}
			const auto element = method->getName();
			const auto type = returnType(*method);
			builder.addField(element, type, false);
			builder.addVirtualMethod(element, method->getSignature(), ACC_PUBLIC, [element, type](Frame& frame_, std::vector<ObjectRef>& args_) {
				auto value = args_.at(0)->getField(element);
				if (type == "J" || type == "D") {
					frame_.setReturnDoubleValue(value->getLongValue());
				} else if (type.size() == 1) {
					frame_.setReturnValue(value->getValue());
				} else {
					frame_.setReturnObject(value);
				}
			});
		}
		Class* type = &annotationType_;
		builder.addVirtualMethod("annotationType", "()Ljava/lang/Class;", ACC_PUBLIC, [&classloader_, type](Frame& frame_, std::vector<ObjectRef>&) {
			frame_.setReturnObject(Object::makeConstClass(classloader_, *type));
		});
		builder.finalize();
		return classloader_.getOrLoad(name);
	}
}  // namespace

ObjectRef sandvik::makeAnnotationInstance(JThread& thread_, const dex::Annotation& annotation_) {
	auto& classloader = thread_.getClassLoader();
	auto& annotationType = classloader.getOrLoad(annotation_.type());
	auto instance = Object::make(implementationClass(classloader, annotationType));
	auto setElement = [&](const std::string& name_, const dex::EncodedValue& value_) {
		if (auto* method = annotationType.getDeclaredMethod(name_, "")) {
			instance->setField(name_, toObject(thread_, value_, returnType(*method)));
		}
	};
	// defaults first, then the values given by the annotation
	if (const auto* defaults = annotationType.getAnnotation("dalvik.annotation.AnnotationDefault")) {
		if (const auto* value = defaults->element("value"); value && value->kind() == dex::EncodedValue::Kind::Annotation) {
			for (const auto& element : value->annotationElements()) {
				setElement(element.name, element.value);
			}
		}
	}
	for (const auto& element : annotation_.elements()) {
		setElement(element.name, element.value);
	}
	return instance;
}
