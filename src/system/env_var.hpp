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
	};  // namespace system
};  // namespace sandvik

#endif /* __SYSTEM_ENVVAR_H__ */
