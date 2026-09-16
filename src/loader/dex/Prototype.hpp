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

#ifndef __SANDVIK_DEX_PROTOTYPE_HPP__
#define __SANDVIK_DEX_PROTOTYPE_HPP__

#include <vector>

#include "Type.hpp"

namespace sandvik {
	namespace dex {
		/** @brief A method prototype: return type plus parameter types, decoded from a proto_id_item.
		 */
		class Prototype {
			public:
				/** @brief Constructs a prototype.
				 * @param returnType_ Return type.
				 * @param params_ Parameter types, in order.
				 */
				Prototype(Type returnType_, std::vector<Type> params_) : _returnType(std::move(returnType_)), _params(std::move(params_)) {
				}

				/** @brief Return type of this prototype. */
				const Type* return_type() const {
					return &_returnType;
				}

				/** @brief Parameter types of this prototype, in order. */
				const std::vector<Type>& parameters_type() const {
					return _params;
				}

			private:
				Type _returnType;
				std::vector<Type> _params;
		};
	}  // namespace dex
}  // namespace sandvik

#endif  // __SANDVIK_DEX_PROTOTYPE_HPP__
