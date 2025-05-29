#ifndef __OBJECT_HPP__
#define __OBJECT_HPP__

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace sandvik {
	class ClassLoader;
	class Class;
	class Object {
		public:
			Object() = default;
			virtual ~Object() = default;
			Object(const Object&);
			Object& operator=(const Object&) = delete;
			Object(Object&& other) noexcept = delete;
			Object& operator=(Object&& other) noexcept = delete;

			void setObjectData(uintptr_t* data_);
			uintptr_t* getObjectData() const;

			virtual std::shared_ptr<Object> clone() const = 0;

			static std::shared_ptr<Object> make(Class& class_);
			static std::shared_ptr<Object> make(uint64_t number_);
			static std::shared_ptr<Object> make(ClassLoader& classloader_, const std::string& str_);
			static std::shared_ptr<Object> make(const std::exception& e_);
			static std::shared_ptr<Object> makeConstClass(ClassLoader& classloader_, Class& classtype_);  // for Class<?> object
			static std::shared_ptr<Object> makeNull();
			static std::shared_ptr<Object> makeVmObject(const std::string& str_);

			virtual std::string debug() const;

			virtual bool isInstanceOf(const std::string& instance_) const;
			virtual bool isNumberObject() const;
			virtual bool isArray() const;
			virtual bool isNull() const;
			virtual uint32_t getArrayLength() const;

			virtual bool operator==(const Object& other) const;
			virtual bool operator==(std::nullptr_t) const;

			// void setField(const std::string& name_, std::shared_ptr<Object>&& value_);
			void setField(const std::string& name_, std::shared_ptr<Object> value_);
			std::shared_ptr<Object> getField(const std::string& name_) const;

		protected:
			std::map<std::string, std::shared_ptr<Object>> _fields;
			std::unique_ptr<uintptr_t> _data;
	};

	class NumberObject : public Object {
		public:
			NumberObject(uint64_t value_);
			~NumberObject() override = default;

			NumberObject(const NumberObject& other);
			NumberObject& operator=(const NumberObject& other);
			std::shared_ptr<Object> clone() const override;

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
			std::shared_ptr<Object> clone() const override;
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
			std::shared_ptr<Object> clone() const override;

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
			std::shared_ptr<Object> clone() const override;

			Class& getClassType() const;
			std::string debug() const override;

			bool operator==(const Object& other) const override;

		private:
			Class& _type;
	};

	class ThrowableObject : public Object {
		public:
			ThrowableObject(const std::exception& e_);
			~ThrowableObject() override = default;

			ThrowableObject(const ThrowableObject& other);
			ThrowableObject& operator=(const ThrowableObject& other);
			std::shared_ptr<Object> clone() const override;

			void throwException();
			std::string debug() const override;

		private:
			std::exception _e;
	};

	class NullObject : public Object {
		public:
			NullObject() = default;
			~NullObject() override = default;

			NullObject(const NullObject& other) = default;
			NullObject& operator=(const NullObject& other) = default;
			std::shared_ptr<Object> clone() const override;

			bool operator==(std::nullptr_t) const override;
			bool isNull() const override;
			std::string debug() const override;
	};
	class VmObject : public Object {
		public:
			VmObject(const std::string& instance_);
			~VmObject() override = default;

			VmObject(const VmObject& other);
			VmObject& operator=(const VmObject& other);
			std::shared_ptr<Object> clone() const override;

			bool isInstanceOf(const std::string& instance_) const override;
			std::string str() const;
			std::string debug() const override;

		private:
			std::string _instance;
	};
}  // namespace sandvik

#endif  // __OBJECT_HPP__
