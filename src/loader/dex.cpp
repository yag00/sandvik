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

#include <algorithm>
#include <fstream>
#include <sstream>
#include <utility>

#include "class.hpp"
#include "classloader.hpp"
#include "dex/Class.hpp"
#include "dex/File.hpp"
#include "field.hpp"
#include "method.hpp"
#include "system/logger.hpp"
#include "types.hpp"
#include "utils.hpp"

using namespace sandvik;
using namespace sandvik::dex;

namespace {
	// Builds a "(ParamDescs)ReturnDesc" method signature directly from a proto_ids entry, the same
	// way utils::get_method_descriptor does from a fully-built dex::Method - but resolveMethod only
	// needs the signature string, not a whole Method object.
	std::string buildSignature(const File& file_, uint32_t protoIdx_) {
		std::ostringstream oss;
		oss << '(';
		for (uint32_t paramTypeIdx : file_.protoParamTypeIdxs(protoIdx_)) {
			oss << get_type_descriptor(Type(file_.typeDescriptor(paramTypeIdx)));
		}
		oss << ')';
		oss << get_type_descriptor(Type(file_.typeDescriptor(file_.protoReturnTypeIdx(protoIdx_))));
		return oss.str();
	}
}  // namespace

Dex::Dex(const std::string& path_) : _path(path_) {
	load(path_);
}
Dex::Dex() = default;

Dex::Dex(std::vector<uint8_t>& buffer, const std::string& path_) : _path(path_) {
	load(buffer);
}

Dex::~Dex() = default;

std::string Dex::getPath() const {
	return _path;
}

void Dex::load(const std::string& path) {
	std::ifstream file(path, std::ios::binary);
	if (!file) {
		throw DexLoaderException("Failed to open DEX file: " + path);
	}
	std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	load(buffer);
}

void Dex::load(std::vector<uint8_t>& buffer) {
	if (buffer.empty()) {
		throw DexLoaderException("Empty buffer provided");
	}
	try {
		_dex = File::parse(std::move(buffer));
	} catch (const std::exception& e) {
		throw DexLoaderException(std::string("DEX parse error: ") + e.what());
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
		names.push_back(cls.pretty_name());
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

void Dex::resolveMethod(uint16_t idx, std::string& class_, std::string& method_, std::string& sig_) const {
	if (!_dex) {
		throw DexLoaderException("No DEX file loaded");
	}

	try {
		if (idx >= _dex->methodIdCount()) {
			throw DexLoaderException(fmt::format("Method index {} out of range", idx));
		}
		class_ = dex::Class(_dex->typeDescriptor(_dex->methodIdClassTypeIdx(idx))).pretty_name();
		method_ = _dex->string(_dex->methodIdNameIdx(idx));
		sig_ = buildSignature(*_dex, _dex->methodIdProtoIdx(idx));
	} catch (const std::exception& e) {
		throw DexLoaderException(fmt::format("Failed to resolve method at index {}: {}", idx, e.what()));
	}
}

void Dex::resolveClass(uint16_t idx, std::string& class_) const {
	if (!_dex) {
		throw DexLoaderException("No DEX file loaded");
	}

	try {
		if (idx >= _dex->typeCount()) {
			throw DexLoaderException(fmt::format("Type index {} out of range", idx));
		}
		Type type(_dex->typeDescriptor(idx));
		switch (type.type()) {
			case Type::TYPES::CLASS:
				class_ = type.cls().pretty_name();
				return;
			case Type::TYPES::PRIMITIVE:
			case Type::TYPES::ARRAY:
				class_ = get_type_descriptor(type);
				return;
			case Type::TYPES::UNKNOWN:
			default:
				throw DexLoaderException(fmt::format("Unknown type at index {}", idx));
		}
	} catch (const std::exception& e) {
		throw DexLoaderException(fmt::format("Failed to resolve class at index {}: {}", idx, e.what()));
	}
}

void Dex::resolveField(uint16_t idx, std::string& class_, std::string& field_) const {
	if (!_dex) {
		throw DexLoaderException("No DEX file loaded");
	}

	try {
		if (idx >= _dex->fieldIdCount()) {
			throw DexLoaderException(fmt::format("Field index {} out of range", idx));
		}
		field_ = _dex->string(_dex->fieldIdNameIdx(idx));
		class_ = dex::Class(_dex->typeDescriptor(_dex->fieldIdClassTypeIdx(idx))).pretty_name();
	} catch (const std::exception& e) {
		throw DexLoaderException(fmt::format("Failed to resolve field at index {}: {}", idx, e.what()));
	}
}

std::string Dex::resolveType(uint16_t idx, TYPES& type_) {
	if (!_dex) {
		throw DexLoaderException("No DEX file loaded");
	}
	if (idx >= _dex->typeCount()) {
		throw DexLoaderException(fmt::format("Type index {} out of range", idx));
	}

	Type type(_dex->typeDescriptor(idx));
	switch (type.type()) {
		case Type::TYPES::CLASS:
			type_ = TYPES::CLASS;
			return type.cls().pretty_name();
		case Type::TYPES::PRIMITIVE:
			type_ = TYPES::PRIMITIVE;
			return get_primitive_type(get_type_descriptor(type));
		case Type::TYPES::ARRAY:
			type_ = TYPES::ARRAY;
			return get_type_descriptor(type);
		case Type::TYPES::UNKNOWN:
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
		if (idx >= _dex->stringCount()) {
			throw DexLoaderException(fmt::format("String index {} out of range", idx));
		}
		return _dex->string(idx);
	} catch (const std::exception& e) {
		throw DexLoaderException(fmt::format("Failed to resolve string at index {}: {}", idx, e.what()));
	}
}

std::vector<std::pair<std::string, uint32_t>> Dex::resolveArray(uint16_t idx) {
	if (!_dex) {
		throw DexLoaderException("No DEX file loaded");
	}

	std::vector<std::pair<std::string, uint32_t>> result;
	try {
		if (idx >= _dex->typeCount()) {
			throw DexLoaderException(fmt::format("Type index {} out of range", idx));
		}
		const std::string& descriptor = _dex->typeDescriptor(idx);
		if (descriptor.empty() || descriptor[0] != '[') {
			throw DexLoaderException(fmt::format("Type at index {} is not an array", idx));
		}

		// Strip the outermost '[' (this array's own dimension). What remains is the descriptor of
		// the immediate element type; count any further leading '[' in it to get how many extra
		// dimensions lie beyond that immediate element (0 for a plain T[], 1 for the outer level of
		// a T[][], etc.) - see Interpreter::new_array, which needs exactly this.
		std::string rest = descriptor.substr(1);
		uint32_t extraDims = 0;
		while (extraDims < rest.size() && rest[extraDims] == '[') {
			++extraDims;
		}
		std::string base = rest.substr(extraDims);
		if (base.empty()) {
			throw DexLoaderException(fmt::format("Malformed array descriptor: {}", descriptor));
		}

		std::string baseName;
		if (base[0] == 'L') {
			if (base.back() != ';') {
				throw DexLoaderException(fmt::format("Malformed class descriptor in array type: {}", base));
			}
			baseName = base.substr(1, base.size() - 2);
			std::replace(baseName.begin(), baseName.end(), '/', '.');
		} else {
			baseName = get_primitive_type(base);
		}
		result.push_back({baseName, extraDims});
	} catch (const std::exception& e) {
		throw DexLoaderException(fmt::format("Failed to resolve array at index {}: {}", idx, e.what()));
	}
	return result;
}
