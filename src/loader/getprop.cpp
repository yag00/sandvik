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

#include "getprop.hpp"

#include <cctype>
#include <fstream>
#include <sstream>

using namespace sandvik;

std::string Getprop::trim(const std::string& value_) {
	size_t first = 0;
	while (first < value_.size() && std::isspace(static_cast<unsigned char>(value_[first])) != 0) {
		++first;
	}

	size_t last = value_.size();
	while (last > first && std::isspace(static_cast<unsigned char>(value_[last - 1])) != 0) {
		--last;
	}

	return value_.substr(first, last - first);
}

bool Getprop::parseLine(const std::string& line_, std::string& keyOut_, std::string& valueOut_) {
	auto line = trim(line_);
	if (line.empty() || line[0] == '#') {
		return false;
	}

	// Android format: [key]: [value]
	if (line.size() >= 7 && line.front() == '[' && line.back() == ']') {
		const auto sep = line.find("]: [");
		if (sep != std::string::npos && sep > 1) {
			keyOut_ = line.substr(1, sep - 1);
			valueOut_ = line.substr(sep + 4, line.size() - (sep + 5));
			return true;
		}
	}

	// Fallback format: key=value
	const auto eq = line.find('=');
	if (eq != std::string::npos && eq > 0) {
		keyOut_ = trim(line.substr(0, eq));
		valueOut_ = trim(line.substr(eq + 1));
		return !keyOut_.empty();
	}

	return false;
}

size_t Getprop::loadFromText(const std::string& output_) {
	std::unordered_map<std::string, std::string> parsed;
	std::istringstream stream(output_);
	std::string line;
	while (std::getline(stream, line)) {
		std::string key;
		std::string value;
		if (parseLine(line, key, value)) {
			parsed[std::move(key)] = std::move(value);
		}
	}

	std::lock_guard lock(_mutex);
	_properties = std::move(parsed);
	return _properties.size();
}

size_t Getprop::loadFromFile(const std::string& path_) {
	std::ifstream in(path_);
	if (!in.is_open()) {
		return 0;
	}

	std::ostringstream buffer;
	buffer << in.rdbuf();
	return loadFromText(buffer.str());
}

std::string Getprop::get(const std::string& key_, const std::string& defaultValue_) const {
	std::lock_guard lock(_mutex);
	auto it = _properties.find(key_);
	if (it == _properties.end()) {
		return defaultValue_;
	}
	return it->second;
}

bool Getprop::has(const std::string& key_) const {
	std::lock_guard lock(_mutex);
	return _properties.find(key_) != _properties.end();
}

void Getprop::clear() {
	std::lock_guard lock(_mutex);
	_properties.clear();
}
