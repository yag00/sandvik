#ifndef __SYSTEM_ENVVAR_HPP__
#define __SYSTEM_ENVVAR_HPP__

#include <string>

namespace sandvik {
	namespace system {
		namespace env {
			/** get value of a variable */
			std::string get(const std::string& name_);
			/** set value of a variable */
			void set(const std::string& name_, const std::string& value_);
		};  // namespace env
	};      // namespace system
};          // namespace sandvik

#endif /* __SYSTEM_ENVVAR_H__ */
