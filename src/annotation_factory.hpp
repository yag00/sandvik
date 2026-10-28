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

#ifndef __ANNOTATION_FACTORY_HPP__
#define __ANNOTATION_FACTORY_HPP__

#include "loader/dex/Annotation.hpp"
#include "object.hpp"

namespace sandvik {
	class JThread;

	/**
	 * @brief Java instance of an annotation read from a dex (Class/Field.getAnnotation).
	 *
	 * ART builds annotation instances with libcore AnnotationFactory over java.lang.reflect.Proxy. Sandvik
	 * synthesizes instead one class per annotation type, implementing the annotation interface: its element methods
	 * return instance fields set from the annotation values, or from the defaults of the annotation type
	 * (dalvik.annotation.AnnotationDefault).
	 * @param thread_ current thread, used to initialize the classes of enum values
	 * @param annotation_ annotation as read from the dex
	 * @return annotation instance
	 */
	ObjectRef makeAnnotationInstance(JThread& thread_, const dex::Annotation& annotation_);
}  // namespace sandvik

#endif  // __ANNOTATION_FACTORY_HPP__
