#ifndef __ARRAY_HPP__
#define __ARRAY_HPP__

#include <memory>
#include <string>
#include <vector>

#include "object.hpp"

namespace sandvik {
	class Array : public Object {
		public:
			static std::shared_ptr<Object> make(const std::string& type_, uint32_t size_);
			explicit Array(const std::string& type_, uint32_t size_);
			Array(const Array& other);
			~Array() = default;
			std::shared_ptr<Object> clone() const override;
			std::string debug() const override;
			uint32_t getArrayLength() const override;

			void setArrayElement(uint32_t index_, std::shared_ptr<Object> value_);
			std::shared_ptr<Object> getArrayElement(uint32_t index_) const;

		private:
			std::string _type;
			uint32_t _size;
			std::vector<std::shared_ptr<Object>> _data;
	};
}  // namespace sandvik

#endif  // __ARRAY_HPP__