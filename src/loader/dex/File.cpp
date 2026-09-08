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

#include "File.hpp"

using namespace sandvik::dex;

namespace {
	constexpr uint32_t NO_INDEX = 0xFFFFFFFFu;

	// Header field byte offsets (header_item is a fixed 0x70-byte layout).
	constexpr size_t OFF_STRING_IDS_SIZE = 56;
	constexpr size_t OFF_STRING_IDS_OFF = 60;
	constexpr size_t OFF_TYPE_IDS_SIZE = 64;
	constexpr size_t OFF_TYPE_IDS_OFF = 68;
	constexpr size_t OFF_PROTO_IDS_SIZE = 72;
	constexpr size_t OFF_PROTO_IDS_OFF = 76;
	constexpr size_t OFF_FIELD_IDS_SIZE = 80;
	constexpr size_t OFF_FIELD_IDS_OFF = 84;
	constexpr size_t OFF_METHOD_IDS_SIZE = 88;
	constexpr size_t OFF_METHOD_IDS_OFF = 92;
	constexpr size_t OFF_CLASS_DEFS_SIZE = 96;
	constexpr size_t OFF_CLASS_DEFS_OFF = 100;
	constexpr size_t HEADER_SIZE = 0x70;

	constexpr size_t CLASS_DEF_ITEM_SIZE = 32;
}  // namespace

std::unique_ptr<File> File::parse(std::vector<uint8_t> buffer_) {
	if (buffer_.size() < HEADER_SIZE) {
		throw DexFormatException("DEX file too small to contain a header");
	}
	// std::unique_ptr<File>(new File()) rather than make_unique: File's constructor is private
	// (construction only ever happens here, via parse()).
	std::unique_ptr<File> file(new File());
	file->_buffer = std::move(buffer_);
	Reader r(file->_buffer.data(), file->_buffer.size());

	if (r.u1(0) != 'd' || r.u1(1) != 'e' || r.u1(2) != 'x' || r.u1(3) != '\n') {
		throw DexFormatException("Not a DEX file (bad magic)");
	}

	const uint32_t stringIdsSize = r.u4(OFF_STRING_IDS_SIZE);
	const uint32_t stringIdsOff = r.u4(OFF_STRING_IDS_OFF);
	const uint32_t typeIdsSize = r.u4(OFF_TYPE_IDS_SIZE);
	const uint32_t typeIdsOff = r.u4(OFF_TYPE_IDS_OFF);
	const uint32_t protoIdsSize = r.u4(OFF_PROTO_IDS_SIZE);
	const uint32_t protoIdsOff = r.u4(OFF_PROTO_IDS_OFF);
	const uint32_t fieldIdsSize = r.u4(OFF_FIELD_IDS_SIZE);
	const uint32_t fieldIdsOff = r.u4(OFF_FIELD_IDS_OFF);
	const uint32_t methodIdsSize = r.u4(OFF_METHOD_IDS_SIZE);
	const uint32_t methodIdsOff = r.u4(OFF_METHOD_IDS_OFF);
	const uint32_t classDefsSize = r.u4(OFF_CLASS_DEFS_SIZE);
	const uint32_t classDefsOff = r.u4(OFF_CLASS_DEFS_OFF);

	file->parseStrings(r, stringIdsOff, stringIdsSize);
	file->parseTypes(r, typeIdsOff, typeIdsSize);
	file->parseProtos(r, protoIdsOff, protoIdsSize);
	file->parseFields(r, fieldIdsOff, fieldIdsSize);
	file->parseMethods(r, methodIdsOff, methodIdsSize);
	file->parseClassDefs(r, classDefsOff, classDefsSize);

	return file;
}

void File::parseStrings(const Reader& r_, uint32_t off_, uint32_t count_) {
	_strings.reserve(count_);
	for (uint32_t i = 0; i < count_; ++i) {
		uint32_t stringDataOff = r_.u4(off_ + static_cast<uint64_t>(i) * 4);
		_strings.push_back(r_.readMUTF8String(stringDataOff));
	}
}

void File::parseTypes(const Reader& r_, uint32_t off_, uint32_t count_) {
	_typeStringIdx.reserve(count_);
	for (uint32_t i = 0; i < count_; ++i) {
		_typeStringIdx.push_back(r_.u4(off_ + static_cast<uint64_t>(i) * 4));
	}
}

std::vector<uint32_t> File::parseTypeList(const Reader& r_, uint32_t off_) const {
	uint32_t size = r_.u4(off_);
	std::vector<uint32_t> result;
	result.reserve(size);
	for (uint32_t i = 0; i < size; ++i) {
		result.push_back(r_.u2(off_ + 4 + static_cast<uint64_t>(i) * 2));
	}
	return result;
}

void File::parseProtos(const Reader& r_, uint32_t off_, uint32_t count_) {
	_protoIds.reserve(count_);
	for (uint32_t i = 0; i < count_; ++i) {
		uint32_t base = off_ + i * 12;
		// shorty_idx at base+0 is a redundant summary of the prototype's descriptor - unused, since
		// return_type_idx/parameters_off already give the exact types.
		uint32_t returnTypeIdx = r_.u4(base + 4);
		uint32_t parametersOff = r_.u4(base + 8);
		std::vector<uint32_t> params = parametersOff != 0 ? parseTypeList(r_, parametersOff) : std::vector<uint32_t>{};
		_protoIds.push_back({returnTypeIdx, std::move(params)});
	}
}

void File::parseFields(const Reader& r_, uint32_t off_, uint32_t count_) {
	_fieldIds.reserve(count_);
	for (uint32_t i = 0; i < count_; ++i) {
		uint32_t base = off_ + i * 8;
		uint32_t classIdx = r_.u2(base);
		uint32_t typeIdx = r_.u2(base + 2);
		uint32_t nameIdx = r_.u4(base + 4);
		_fieldIds.push_back({classIdx, typeIdx, nameIdx});
	}
}

void File::parseMethods(const Reader& r_, uint32_t off_, uint32_t count_) {
	_methodIds.reserve(count_);
	for (uint32_t i = 0; i < count_; ++i) {
		uint32_t base = off_ + i * 8;
		uint32_t classIdx = r_.u2(base);
		uint32_t protoIdx = r_.u2(base + 2);
		uint32_t nameIdx = r_.u4(base + 4);
		_methodIds.push_back({classIdx, protoIdx, nameIdx});
	}
}

const std::string& File::string(uint32_t idx_) const {
	return _strings.at(idx_);
}

const std::string& File::typeDescriptor(uint32_t idx_) const {
	return string(_typeStringIdx.at(idx_));
}

uint32_t File::methodIdClassTypeIdx(uint32_t idx_) const {
	return _methodIds.at(idx_).classTypeIdx;
}
uint32_t File::methodIdProtoIdx(uint32_t idx_) const {
	return _methodIds.at(idx_).protoIdx;
}
uint32_t File::methodIdNameIdx(uint32_t idx_) const {
	return _methodIds.at(idx_).nameIdx;
}

uint32_t File::fieldIdClassTypeIdx(uint32_t idx_) const {
	return _fieldIds.at(idx_).classTypeIdx;
}
uint32_t File::fieldIdTypeIdx(uint32_t idx_) const {
	return _fieldIds.at(idx_).typeIdx;
}
uint32_t File::fieldIdNameIdx(uint32_t idx_) const {
	return _fieldIds.at(idx_).nameIdx;
}

uint32_t File::protoReturnTypeIdx(uint32_t protoIdx_) const {
	return _protoIds.at(protoIdx_).returnTypeIdx;
}
const std::vector<uint32_t>& File::protoParamTypeIdxs(uint32_t protoIdx_) const {
	return _protoIds.at(protoIdx_).paramTypeIdxs;
}

Type File::resolveType(uint32_t typeIdx_) const {
	return Type(typeDescriptor(typeIdx_));
}

Prototype File::resolveProto(uint32_t protoIdx_) const {
	const auto& proto = _protoIds.at(protoIdx_);
	std::vector<Type> params;
	params.reserve(proto.paramTypeIdxs.size());
	for (uint32_t t : proto.paramTypeIdxs) {
		params.push_back(resolveType(t));
	}
	return Prototype(resolveType(proto.returnTypeIdx), std::move(params));
}

std::unique_ptr<Field> File::buildField(uint32_t fieldIdx_, uint32_t accessFlags_) const {
	const auto& fid = _fieldIds.at(fieldIdx_);
	return std::make_unique<Field>(string(fid.nameIdx), resolveType(fid.typeIdx), accessFlags_, fieldIdx_);
}

std::unique_ptr<Method> File::buildMethod(const Reader& r_, uint32_t methodIdx_, uint32_t accessFlags_, bool isVirtual_, uint32_t codeOff_) const {
	const auto& mid = _methodIds.at(methodIdx_);
	Prototype proto = resolveProto(mid.protoIdx);

	std::vector<uint8_t> bytecode;
	CodeInfo codeInfo;
	if (codeOff_ != 0) {
		size_t cursor = codeOff_;
		uint16_t registersSize = r_.readU2(cursor);
		r_.readU2(cursor);  // ins_size: unused (register layout is derived from bytecode itself)
		r_.readU2(cursor);  // outs_size: unused (only matters for verification, which sandvik doesn't do)
		uint16_t triesSize = r_.readU2(cursor);
		r_.readU4(cursor);  // debug_info_off: unused (no line-number/local-variable debugging)
		uint32_t insnsSize = r_.readU4(cursor);

		codeInfo.setNbRegisters(registersSize);

		size_t insnsByteOff = cursor;
		size_t insnsByteLen = static_cast<size_t>(insnsSize) * 2;
		r_.checkBounds(insnsByteOff, insnsByteLen);
		bytecode.assign(r_.data() + insnsByteOff, r_.data() + insnsByteOff + insnsByteLen);

		if (triesSize != 0) {
			size_t triesOffset = insnsByteOff + insnsByteLen;
			if ((insnsSize % 2) != 0) {
				triesOffset += 2;  // pad to a 4-byte boundary before the tries table
			}

			struct RawTryItem {
					uint32_t startAddr;
					uint16_t insnCount;
					uint16_t handlerOff;
			};
			std::vector<RawTryItem> tries;
			tries.reserve(triesSize);
			size_t tp = triesOffset;
			for (uint16_t i = 0; i < triesSize; ++i) {
				RawTryItem t;
				t.startAddr = r_.readU4(tp);
				t.insnCount = r_.readU2(tp);
				t.handlerOff = r_.readU2(tp);
				tries.push_back(t);
			}
			// tp now sits right past the last try_item: the start of encoded_catch_handler_list,
			// against which each try_item's handler_off is relative.
			size_t handlerListBase = tp;

			for (const auto& t : tries) {
				trycatch_item item;
				item.start_addr = t.startAddr;
				item.insn_count = t.insnCount;
				item.catch_all_addr = 0;

				size_t hp = handlerListBase + t.handlerOff;
				int32_t size = r_.readSLEB128(hp);
				int32_t numCatches = size < 0 ? -size : size;
				for (int32_t i = 0; i < numCatches; ++i) {
					uint32_t typeIdx = r_.readULEB128(hp);
					uint32_t addr = r_.readULEB128(hp);
					item.handlers.emplace_back(typeIdx, addr);
				}
				if (size <= 0) {
					item.catch_all_addr = r_.readULEB128(hp);
				}
				codeInfo.exceptions().push_back(std::move(item));
			}
		}
	}

	return std::make_unique<Method>(string(mid.nameIdx), std::move(proto), accessFlags_, isVirtual_, methodIdx_, std::move(bytecode), std::move(codeInfo));
}

std::unique_ptr<Class> File::parseClassDef(const Reader& r_, uint32_t classDefOffset_, size_t index_) {
	uint32_t classIdx = r_.u4(classDefOffset_);
	uint32_t accessFlags = r_.u4(classDefOffset_ + 4);
	uint32_t superclassIdx = r_.u4(classDefOffset_ + 8);
	uint32_t interfacesOff = r_.u4(classDefOffset_ + 12);
	// source_file_idx (+16) and annotations_off (+20): unused, no debug/annotation support.
	uint32_t classDataOff = r_.u4(classDefOffset_ + 24);
	// static_values_off (+28): unused - static fields are already default-initialized elsewhere
	// (see Field's value storage), and encoded initial values are an optimization sandvik doesn't
	// need to replicate (a <clinit>, when present, assigns the real values anyway).

	std::string descriptor = typeDescriptor(classIdx);
	std::string superDescriptor;
	if (superclassIdx != NO_INDEX) {
		superDescriptor = typeDescriptor(superclassIdx);
	}

	std::vector<std::string> interfaces;
	if (interfacesOff != 0) {
		auto typeIdxs = parseTypeList(r_, interfacesOff);
		interfaces.reserve(typeIdxs.size());
		for (uint32_t t : typeIdxs) {
			interfaces.push_back(Class(typeDescriptor(t)).pretty_name());
		}
	}

	std::vector<std::unique_ptr<Field>> fields;
	std::vector<std::unique_ptr<Method>> methods;
	if (classDataOff != 0) {
		size_t cursor = classDataOff;
		uint32_t staticFieldsSize = r_.readULEB128(cursor);
		uint32_t instanceFieldsSize = r_.readULEB128(cursor);
		uint32_t directMethodsSize = r_.readULEB128(cursor);
		uint32_t virtualMethodsSize = r_.readULEB128(cursor);

		fields.reserve(static_cast<size_t>(staticFieldsSize) + instanceFieldsSize);
		methods.reserve(static_cast<size_t>(directMethodsSize) + virtualMethodsSize);

		uint32_t fieldIdx = 0;
		for (uint32_t i = 0; i < staticFieldsSize; ++i) {
			fieldIdx += r_.readULEB128(cursor);
			uint32_t fieldAccessFlags = r_.readULEB128(cursor);
			fields.push_back(buildField(fieldIdx, fieldAccessFlags));
		}
		fieldIdx = 0;
		for (uint32_t i = 0; i < instanceFieldsSize; ++i) {
			fieldIdx += r_.readULEB128(cursor);
			uint32_t fieldAccessFlags = r_.readULEB128(cursor);
			fields.push_back(buildField(fieldIdx, fieldAccessFlags));
		}
		uint32_t methodIdx = 0;
		for (uint32_t i = 0; i < directMethodsSize; ++i) {
			methodIdx += r_.readULEB128(cursor);
			uint32_t methodAccessFlags = r_.readULEB128(cursor);
			uint32_t codeOff = r_.readULEB128(cursor);
			methods.push_back(buildMethod(r_, methodIdx, methodAccessFlags, /*isVirtual=*/false, codeOff));
		}
		methodIdx = 0;
		for (uint32_t i = 0; i < virtualMethodsSize; ++i) {
			methodIdx += r_.readULEB128(cursor);
			uint32_t methodAccessFlags = r_.readULEB128(cursor);
			uint32_t codeOff = r_.readULEB128(cursor);
			methods.push_back(buildMethod(r_, methodIdx, methodAccessFlags, /*isVirtual=*/true, codeOff));
		}
	}

	return std::make_unique<Class>(std::move(descriptor), accessFlags, std::move(superDescriptor), std::move(methods), std::move(fields), std::move(interfaces),
	                               index_);
}

void File::parseClassDefs(const Reader& r_, uint32_t off_, uint32_t count_) {
	_classes.reserve(count_);
	for (uint32_t i = 0; i < count_; ++i) {
		_classes.push_back(parseClassDef(r_, off_ + i * CLASS_DEF_ITEM_SIZE, i));
	}
}
