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

#ifndef __OBJECT_HPP__
#define __OBJECT_HPP__

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace sandvik {
	class ClassLoader;
	class Class;
	class Object;
	class NumberObject;
	class StringObject;

	using ObjectRef = std::shared_ptr<Object>;

	class Object {
		public:
			Object() = default;
			virtual ~Object() = default;
			Object(const Object&) = delete;
			Object& operator=(const Object&) = delete;
			Object(Object&& other) noexcept = delete;
			Object& operator=(Object&& other) noexcept = delete;

			static ObjectRef make(Class& class_);
			static ObjectRef make(uint64_t number_);
			static ObjectRef make(ClassLoader& classloader_, const std::string& str_);
			static ObjectRef makeConstClass(ClassLoader& classloader_, Class& classtype_);  // for Class<?> object
			static ObjectRef makeNull();
			static ObjectRef makeArray(ClassLoader& classloader_, const Class& classtype_, const std::vector<uint32_t>& dimensions_);

			virtual std::string debug() const;

			virtual bool isInstanceOf(const std::string& instance_) const;
			virtual bool isNumberObject() const;
			virtual bool isArray() const;
			virtual bool isNull() const;
			virtual uint32_t getArrayLength() const;

			virtual bool operator==(const Object& other) const;
			virtual bool operator==(std::nullptr_t) const;

			void setField(const std::string& name_, ObjectRef value_);
			ObjectRef getField(const std::string& name_) const;

		protected:
			std::map<std::string, ObjectRef> _fields;
	};

	class NumberObject : public Object {
		public:
			NumberObject(uint64_t value_);
			~NumberObject() override = default;

			NumberObject(const NumberObject& other);
			NumberObject& operator=(const NumberObject& other);

			bool isNumberObject() const override;
			int32_t getValue() const;
			int64_t getLongValue() const;
			float getFloatValue() const;
			std::string debug() const override;

			bool operator==(const Object& other) const override;

		private:
			uint64_t _value;
	};

	class ObjectClass : public Object {
		public:
			ObjectClass(Class& class_);
			~ObjectClass() override = default;

			ObjectClass(const ObjectClass& other);
			ObjectClass& operator=(const ObjectClass& other);
			bool isInstanceOf(const std::string& instance_) const override;

			Class& getClass() const;
			std::string debug() const override;

		private:
			Class& _class;
	};

	class StringObject : public ObjectClass {
		public:
			StringObject(Class& class_, const std::string& value_);
			~StringObject() override = default;

			StringObject(const StringObject& other);
			StringObject& operator=(const StringObject& other);

			void set(const std::string& str_);
			std::string str() const;
			std::string debug() const override;

			bool operator==(const Object& other) const override;

		private:
			std::string _value;
	};
	class ConstClassObject : public ObjectClass {
		public:
			ConstClassObject(Class& class_, Class& type_);
			~ConstClassObject() override = default;

			ConstClassObject(const ConstClassObject& other);
			ConstClassObject& operator=(const ConstClassObject& other);

			Class& getClassType() const;
			std::string debug() const override;

			bool operator==(const Object& other) const override;

		private:
			Class& _type;
	};

	class NullObject : public Object {
		public:
			NullObject() = default;
			~NullObject() override = default;

			NullObject(const NullObject& other) = default;
			NullObject& operator=(const NullObject& other) = default;

			bool operator==(std::nullptr_t) const override;
			bool isNull() const override;
			std::string debug() const override;
	};
}  // namespace sandvik

#endif  // __OBJECT_HPP__
