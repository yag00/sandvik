#ifndef __HEAP_HPP__
#define __HEAP_HPP__

#include <stdint.h>

#include <memory>
#include <vector>

namespace sandvik {
	class Class;
	class Object;
	class Heap {
		public:
			Heap();
			~Heap() = default;
			Heap(const Heap&) = delete;
			Heap& operator=(const Heap&) = delete;
			Heap(Heap&&) = delete;
			Heap& operator=(Heap&&) = delete;

			std::shared_ptr<Object> allocateObject(Class& class_);

		private:
			std::vector<std::shared_ptr<Object>> _objects;
	};
}  // namespace sandvik
#endif  // __HEAP_HPP__