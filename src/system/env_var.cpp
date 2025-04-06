#include "env_var.hpp"

#include <fmt/format.h>

#include <cstdlib>

using namespace sandvik::system;

std::string env::get(const std::string& name_) {
	const char* val = std::getenv(name_.c_str());
	if (val) return std::string(val);
	return "";
}

void env::set(const std::string& name_, const std::string& value_) {
	if (setenv(name_.c_str(), value_.c_str(), 1) != 0) throw std::runtime_error(fmt::format("Can't set {} environment variable!", name_));
}
