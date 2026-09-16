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

#include "Class.hpp"

#include <algorithm>

using namespace sandvik::dex;

namespace {
	/** Demangles "Lfoo/bar/Baz;" to "foo.bar.Baz". Non-class-shaped descriptors pass through as-is
	 * (defensive: this project's Class is only ever built from class-shaped descriptors, but a
	 * class-def's own descriptor is always well-formed by construction, so this is really just a
	 * safety net, never expected to trigger the fallback branch). */
	std::string demangle(const std::string& descriptor_) {
		if (descriptor_.size() >= 2 && descriptor_.front() == 'L' && descriptor_.back() == ';') {
			std::string inner = descriptor_.substr(1, descriptor_.size() - 2);
			std::replace(inner.begin(), inner.end(), '/', '.');
			return inner;
		}
		return descriptor_;
	}
}  // namespace

std::string Class::pretty_name() const {
	return demangle(_descriptor);
}

std::string Class::name() const {
	std::string pretty = pretty_name();
	auto pos = pretty.find_last_of('.');
	return pos == std::string::npos ? pretty : pretty.substr(pos + 1);
}

std::string Class::package_name() const {
	std::string pretty = pretty_name();
	auto pos = pretty.find_last_of('.');
	return pos == std::string::npos ? std::string() : pretty.substr(0, pos);
}
