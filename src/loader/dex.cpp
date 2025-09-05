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

#include "dex.hpp"

#include <fmt/format.h>

#include <LIEF/DEX/Class.hpp>
#include <LIEF/DEX/Field.hpp>
#include <LIEF/DEX/File.hpp>
#include <LIEF/DEX/Method.hpp>
#include <LIEF/DEX/Parser.hpp>
#include <fstream>
#include <utility>

#include "class.hpp"
#include "classloader.hpp"
#include "field.hpp"
#include "method.hpp"
#include "system/logger.hpp"
#include "types.hpp"
#include "utils.hpp"

using namespace sandvik;
using namespace LIEF::DEX;

Dex::Dex(const std::string& path_) : _path(path_) {
	load(path_);
}
Dex::Dex() {
}

Dex::Dex(std::vector<uint8_t>& buffer, const std::string& name) {
	load(buffer);
}

Dex::~Dex() = default;

std::string Dex::getPath() const {
	return _path;
}

void Dex::load(const std::string& path) {
	try {
		_dex = Parser::parse(path);
		if (!_dex) {
			throw DexLoaderException("Failed to parse DEX file: " + path);
		}
	} catch (const std::exception& e) {
		throw DexLoaderException(std::string("LIEF error: ") + e.what());
	}
}

void Dex::load(std::vector<uint8_t>& buffer, const std::string& name) {
	if (buffer.empty()) {
		throw DexLoaderException("Empty buffer provided");
	}

	try {
		_dex = Parser::parse(buffer);
		if (!_dex) {
			throw DexLoaderException("Failed to parse DEX from moved buffer");
		}
	} catch (const std::exception& e) {
		throw DexLoaderException(std::string("LIEF error: ") + e.what());
	}
}

bool Dex::is_loaded() const noexcept {
	return _dex != nullptr;
}

std::vector<std::string> Dex::getClassNames() const {
	if (!_dex) {
		throw DexLoaderException("No DEX file loaded");
	}

	std::vector<std::string> names;
	for (const auto& cls : _dex->classes()) {
		names.push_back(cls.fullname());
	}
	return names;
}

std::unique_ptr<::sandvik::Class> Dex::findClass(ClassLoader& classloader_, const std::string& name) const {
	if (!_dex) {
		throw DexLoaderException("No DEX file loaded");
	}

	for (const auto& cls : _dex->classes()) {
		if (cls.pretty_name() == name) {
			return std::make_unique<::sandvik::Class>(classloader_, classloader_.getDexIndex(*this), cls);
		}
	}
	throw DexLoaderException(fmt::format("Class '{}' not found in DEX file", name));
}

void Dex::resolveMethod(uint16_t idx, std::string& class_, std::string& method_, std::string& sig_) {
	if (!_dex) {
		throw DexLoaderException("No DEX file loaded");
	}

	try {
		const auto& methods = _dex->methods();
		if (idx >= methods.size()) {
			throw DexLoaderException(fmt::format("Method index {} out of range", idx));
		}
		auto it = methods.begin();
		std::advance(it, idx);
		const auto& method = *it;

		class_ = method.cls()->pretty_name();
		method_ = method.name();
		sig_ = get_method_descriptor(method);
	} catch (const std::exception& e) {
		throw DexLoaderException(fmt::format("Failed to resolve method at index {}: {}", idx, e.what()));
	}
}

void Dex::resolveClass(uint16_t idx, std::string& class_) {
	if (!_dex) {
		throw DexLoaderException("No DEX file loaded");
	}

	try {
		// Get the type_id item first
		const auto& type_items = _dex->types();
		if (idx >= type_items.size()) {
			throw DexLoaderException(fmt::format("Type index {} out of range", idx));
		}

		// Get the specific type
		const auto& type = type_items[idx];

		switch (type.type()) {
			case LIEF::DEX::Type::TYPES::CLASS:
				class_ = type.cls().pretty_name();
				return;
			case LIEF::DEX::Type::TYPES::PRIMITIVE:
				class_ = get_type_descriptor(type);
				return;
			case LIEF::DEX::Type::TYPES::ARRAY:
				class_ = get_type_descriptor(type);
				return;
			case LIEF::DEX::Type::TYPES::UNKNOWN:
			default:
				throw DexLoaderException(fmt::format("Unknown type at index {}", idx));
		}
	} catch (const std::exception& e) {
		throw DexLoaderException(fmt::format("Failed to resolve class at index {}: {}", idx, e.what()));
	}
}
void Dex::resolveField(uint16_t idx, std::string& class_, std::string& field_) {
	if (!_dex) {
		throw DexLoaderException("No DEX file loaded");
	}

	try {
		const auto& fields = _dex->fields();
		if (idx >= fields.size()) {
			throw DexLoaderException(fmt::format("Field index {} out of range", idx));
		}

		auto it = fields.begin();
		std::advance(it, idx);
		field_ = it->name();
		class_ = it->cls()->pretty_name();
	} catch (const std::exception& e) {
		throw DexLoaderException(fmt::format("Failed to resolve field at index {}: {}", idx, e.what()));
	}
}

std::string Dex::resolveType(uint16_t idx, TYPES& type_) {
	if (!_dex) {
		throw DexLoaderException("No DEX file loaded");
	}
	// Get the type_id item first
	const auto& type_items = _dex->types();
	if (idx >= type_items.size()) {
		throw DexLoaderException(fmt::format("Type index {} out of range", idx));
	}

	// Get the specific type
	const auto& type = type_items[idx];

	switch (type.type()) {
		case LIEF::DEX::Type::TYPES::CLASS:
			type_ = TYPES::CLASS;
			return type.cls().pretty_name();
		case LIEF::DEX::Type::TYPES::PRIMITIVE:
			type_ = TYPES::PRIMITIVE;
			return get_primitive_type(get_type_descriptor(type));
		case LIEF::DEX::Type::TYPES::ARRAY:
			type_ = TYPES::ARRAY;
			return get_type_descriptor(type);
		case LIEF::DEX::Type::TYPES::UNKNOWN:
		default:
			type_ = TYPES::UNKNOWN;
			return "<unknown>";
	}
}

std::string Dex::resolveString(uint16_t idx) {
	if (!_dex) {
		throw DexLoaderException("No DEX file loaded");
	}

	try {
		const auto& strings = _dex->strings();
		if (idx >= strings.size()) {
			throw DexLoaderException(fmt::format("String index {} out of range", idx));
		}

		auto it = strings.begin();
		std::advance(it, idx);
		return *it;
	} catch (const std::exception& e) {
		throw DexLoaderException(fmt::format("Failed to resolve string at index {}: {}", idx, e.what()));
	}
}

std::vector<std::pair<std::string, uint32_t>> Dex::resolveArray(uint16_t idx) {
	if (!_dex) {
		throw DexLoaderException("No DEX file loaded");
	}

	std::vector<std::pair<std::string, uint32_t>> _array;
	try {
		// Get the type_id item first
		const auto& type_items = _dex->types();
		if (idx >= type_items.size()) {
			throw DexLoaderException(fmt::format("Type index {} out of range", idx));
		}

		// Get the specific type
		const auto& type = type_items[idx];
		if (type.type() != LIEF::DEX::Type::TYPES::ARRAY) {
			throw DexLoaderException(fmt::format("Type at index {} is not a array", idx));
		}
		// remove first [
		auto descriptor = get_type_descriptor(type).substr(1);
		for (const auto& item : type.array()) {
			switch (item.type()) {
				case LIEF::DEX::Type::TYPES::PRIMITIVE:
					_array.push_back({get_primitive_type(descriptor), item.dim()});
					break;
				case LIEF::DEX::Type::TYPES::CLASS: {
					if (!descriptor.empty()) {
						if (descriptor.front() == 'L') {
							descriptor = descriptor.substr(1);  // remove 'L'
						} else {
							logger.ferror("Expected class descriptor to start with 'L', got '{}'", descriptor);
						}
						if (descriptor.back() == ';') {
							descriptor.pop_back();
						} else {
							logger.ferror("Expected class descriptor to end with ';', got '{}'", descriptor);
						}
						std::replace(descriptor.begin(), descriptor.end(), '/', '.');
						_array.push_back({descriptor, item.dim()});
					} else {
						throw DexLoaderException("Empty class descriptor in array type");
					}
					break;
				}
				case LIEF::DEX::Type::TYPES::ARRAY:
					throw DexLoaderException(fmt::format("Nested array type: {} [{}] not supported", get_type_descriptor(item), item.dim()));
				case LIEF::DEX::Type::TYPES::UNKNOWN:
				default:
					throw DexLoaderException(fmt::format("Unknown type: {} [{}] not supported", get_type_descriptor(item), item.dim()));
			}
		}
	} catch (const std::exception& e) {
		throw DexLoaderException(fmt::format("Failed to resolve string at index {}: {}", idx, e.what()));
	}
	return _array;
}