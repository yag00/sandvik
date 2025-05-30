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

#ifndef __JAVA_BASE_CLASSES_HPP__
#define __JAVA_BASE_CLASSES_HPP__

#include <ostream>
#include <string>

namespace sandvik {
	class ClassLoader;
	class Vm;
}  // namespace sandvik

namespace java {
	namespace io {
		void PrintStream(::sandvik::ClassLoader& classLoader);
	}  // namespace io
	namespace lang {
		void Class(::sandvik::ClassLoader& classLoader);
		void Double(::sandvik::ClassLoader& classLoader);
		void IllegalArgumentException(::sandvik::ClassLoader& classLoader);
		void Integer(::sandvik::ClassLoader& classLoader);
		void Math(::sandvik::ClassLoader& classLoader);
		void Object(::sandvik::ClassLoader& classLoader);
		void Package(::sandvik::ClassLoader& classLoader);
		void RuntimeException(::sandvik::ClassLoader& classLoader);
		void String(::sandvik::ClassLoader& classLoader);
		void StringBuilder(::sandvik::ClassLoader& classLoader);
		void System(::sandvik::Vm& vm, ::sandvik::ClassLoader& classLoader);
	}  // namespace lang
}  // namespace java

#endif  // __JAVA_BASE_CLASSES_HPP__