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

#ifndef __SANDVIK_DEX_ITERATOR_HPP__
#define __SANDVIK_DEX_ITERATOR_HPP__

#include <cstddef>
#include <memory>
#include <vector>

namespace sandvik {
	namespace dex {
		/** @brief A dereferencing view over a std::vector<std::unique_ptr<T>>, so callers can
		 * range-for over T& instead of the owning unique_ptr<T>. Deliberately minimal (this project's
		 * only consumer pattern is "range-for" and "size()") rather than a full iterator-category
		 * implementation.
		 *
		 * PtrRange itself does not own the pointed-to elements; it is a thin, cheap-to-copy view over
		 * a container owned elsewhere (typically a File or Class), valid only as long as that
		 * container's storage is not reallocated.
		 */
		template <typename T>
		class PtrRange {
			public:
				/** @brief The underlying owning container type. */
				using container_t = std::vector<std::unique_ptr<T>>;

				/** @brief A forward iterator dereferencing to T& instead of unique_ptr<T>&. */
				class iterator {
					public:
						/** @brief Wraps a raw container iterator. */
						explicit iterator(typename container_t::const_iterator it_) : _it(it_) {
						}
						/** @brief Dereferences to the pointed-to element, not the owning unique_ptr. */
						T& operator*() const {
							return **_it;
						}
						/** @brief Advances to the next element. */
						iterator& operator++() {
							++_it;
							return *this;
						}
						/** @brief Inequality, for range-for's loop condition. */
						bool operator!=(const iterator& other_) const {
							return _it != other_._it;
						}
						/** @brief Equality. */
						bool operator==(const iterator& other_) const {
							return _it == other_._it;
						}

					private:
						typename container_t::const_iterator _it;
				};

				/** @brief Wraps a container, borrowing (not owning) it.
				 * @param c_ Container to view. Must outlive this PtrRange.
				 */
				explicit PtrRange(const container_t& c_) : _c(c_) {
				}
				/** @brief Iterator to the first element. */
				iterator begin() const {
					return iterator(_c.begin());
				}
				/** @brief Iterator past the last element. */
				iterator end() const {
					return iterator(_c.end());
				}
				/** @brief Number of elements. */
				size_t size() const {
					return _c.size();
				}
				/** @brief Indexed access, dereferencing to T& instead of unique_ptr<T>&.
				 * @param idx_ Index of the element.
				 * @return Reference to the element.
				 */
				T& operator[](size_t idx_) const {
					return *_c[idx_];
				}

			private:
				const container_t& _c;
		};
	}  // namespace dex
}  // namespace sandvik

#endif  // __SANDVIK_DEX_ITERATOR_HPP__
