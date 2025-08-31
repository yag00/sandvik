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

#ifndef __JNIHANDLEMAP_HPP__
#define __JNIHANDLEMAP_HPP__

#include <memory>
#include <string>
#include <unordered_map>

#include "jni/jni.h"

namespace sandvik {
	class Object;
	class JNIHandleMap {
		public:
			jobject toJObject(const std::shared_ptr<Object>& obj);

			std::shared_ptr<Object> fromJObject(jobject handle);

			void release(jobject handle);

		private:
			std::unordered_map<jobject, std::shared_ptr<Object>> _table;
	};
}  // namespace sandvik

#endif  // __JNIHANDLEMAP_HPP__