#include "heap.hpp"

#include <fmt/format.h>

#include "class.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

Heap::Heap() {
}

std::shared_ptr<Object> Heap::allocateObject(Class& class_) {
	auto object = Object::make(class_);
	_objects.push_back(std::move(object));
	logger.debug(fmt::format("Heap::allocateObject {} --> ref {}", class_.getName(), _objects.size()));
	return _objects.back();
}