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

#ifndef __SANDVIK_VERSION_H__
#define __SANDVIK_VERSION_H__

#include <string>

namespace sandvik {
	/** @ingroup groupConfig
	 * @brief sandvik build & version information */
	namespace version {
		/** @return sandvik version */
		const std::string getVersion();
		/** @return the commit used to build sandvik */
		const std::string getShortCommit();
		/** @return the revision/commit used to build sandvik */
		const std::string getCommit();
		/** @return the build date of sandvik */
		const std::string getBuildDate();
	};  // namespace version
};  // namespace sandvik

#endif /* __SANDVIK_VERSION_H__ */
