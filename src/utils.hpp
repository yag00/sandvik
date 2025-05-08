#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>

namespace LIEF {
	namespace DEX {
		class Type;
		class Method;
	}  // namespace DEX
}  // namespace LIEF

namespace sandvik {
	std::string get_type_descriptor(const LIEF::DEX::Type& type);
	std::string get_method_descriptor(const LIEF::DEX::Method& method);
}  // namespace sandvik

#endif  // __CLASS_HPP__