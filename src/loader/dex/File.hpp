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

#ifndef __SANDVIK_DEX_FILE_HPP__
#define __SANDVIK_DEX_FILE_HPP__

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Annotation.hpp"
#include "Class.hpp"
#include "iterator.hpp"
#include "reader.hpp"

namespace sandvik {
	namespace dex {
		/** @brief A fully-parsed DEX file */
		class File {
			public:
				/** @brief Parses a complete DEX file image.
				 * @param buffer_ Raw file bytes (moved in and owned by the returned File).
				 * @return The parsed File.
				 * @throw DexFormatException if the buffer is not a well-formed DEX file.
				 */
				static std::unique_ptr<File> parse(std::vector<uint8_t> buffer_);

				/** @brief All classes defined (class_def_item) in this DEX file. */
				PtrRange<Class> classes() const {
					return PtrRange<Class>(_classes);
				}

				/** @brief Number of entries in the string pool. */
				size_t stringCount() const {
					return _strings.size();
				}
				/** @brief Resolves a string_ids index to its decoded (UTF-8) value. */
				const std::string& string(uint32_t idx_) const;

				/** @brief Number of entries in the type pool. */
				size_t typeCount() const {
					return _typeStringIdx.size();
				}
				/** @brief Resolves a type_ids index to its descriptor string (e.g. "I", "[Ljava/lang/String;"). */
				const std::string& typeDescriptor(uint32_t idx_) const;

				/** @brief Number of entries in the method_ids table. */
				size_t methodIdCount() const {
					return _methodIds.size();
				}
				/** @brief Resolves a method_ids index to its declaring class's type_ids index. */
				uint32_t methodIdClassTypeIdx(uint32_t idx_) const;
				/** @brief Resolves a method_ids index to its proto_ids index. */
				uint32_t methodIdProtoIdx(uint32_t idx_) const;
				/** @brief Resolves a method_ids index to its name's string_ids index. */
				uint32_t methodIdNameIdx(uint32_t idx_) const;

				/** @brief Number of entries in the field_ids table. */
				size_t fieldIdCount() const {
					return _fieldIds.size();
				}
				/** @brief Resolves a field_ids index to its declaring class's type_ids index. */
				uint32_t fieldIdClassTypeIdx(uint32_t idx_) const;
				/** @brief Resolves a field_ids index to its type's type_ids index. */
				uint32_t fieldIdTypeIdx(uint32_t idx_) const;
				/** @brief Resolves a field_ids index to its name's string_ids index. */
				uint32_t fieldIdNameIdx(uint32_t idx_) const;

				/** @brief Resolves a proto_ids index to its return-type type_ids index. */
				uint32_t protoReturnTypeIdx(uint32_t protoIdx_) const;
				/** @brief Resolves a proto_ids index to its parameter type_ids indices, in order. */
				const std::vector<uint32_t>& protoParamTypeIdxs(uint32_t protoIdx_) const;

			private:
				File() = default;

				struct ProtoIdRaw {
						uint32_t returnTypeIdx;
						std::vector<uint32_t> paramTypeIdxs;
				};
				struct FieldIdRaw {
						uint32_t classTypeIdx;
						uint32_t typeIdx;
						uint32_t nameIdx;
				};
				struct MethodIdRaw {
						uint32_t classTypeIdx;
						uint32_t protoIdx;
						uint32_t nameIdx;
				};

				void parseStrings(const Reader& r_, uint32_t off_, uint32_t count_);
				void parseTypes(const Reader& r_, uint32_t off_, uint32_t count_);
				void parseProtos(const Reader& r_, uint32_t off_, uint32_t count_);
				void parseFields(const Reader& r_, uint32_t off_, uint32_t count_);
				void parseMethods(const Reader& r_, uint32_t off_, uint32_t count_);
				void parseClassDefs(const Reader& r_, uint32_t off_, uint32_t count_);
				std::unique_ptr<Class> parseClassDef(const Reader& r_, uint32_t classDefOffset_, size_t index_);
				std::unique_ptr<Field> buildField(uint32_t fieldIdx_, uint32_t accessFlags_) const;
				std::unique_ptr<Method> buildMethod(const Reader& r_, uint32_t methodIdx_, uint32_t accessFlags_, bool isVirtual_, uint32_t codeOff_) const;
				Type resolveType(uint32_t typeIdx_) const;
				Prototype resolveProto(uint32_t protoIdx_) const;
				std::vector<uint32_t> parseTypeList(const Reader& r_, uint32_t off_) const;

				/** @brief Decodes one encoded_value at cursor_, advancing it past the encoding.
				 * Handles every encoded_value tag (not just the ones this project's callers
				 * currently read), since a tag this project doesn't care about still has to be
				 * skipped correctly or a later sibling element misparses. */
				EncodedValue parseEncodedValue(const Reader& r_, size_t& cursor_) const;
				/** @brief Decodes an encoded_annotation's type_idx + elements at cursor_, advancing
				 * it past the encoding. Shared by parseAnnotationItem() (which has its own leading
				 * visibility byte) and parseEncodedValue()'s VALUE_ANNOTATION case (which doesn't). */
				std::pair<std::string, std::vector<AnnotationElement>> parseEncodedAnnotationBody(const Reader& r_, size_t& cursor_) const;
				/** @brief Decodes one annotation_item (a visibility byte + encoded_annotation) at offset_. */
				Annotation parseAnnotationItem(const Reader& r_, uint32_t offset_) const;
				/** @brief Decodes an annotation_set_item at offset_ into its list of annotations. */
				std::vector<Annotation> parseAnnotationSetItem(const Reader& r_, uint32_t offset_) const;
				/** @brief Decodes an annotations_directory_item at offset_, attaching the decoded
				 * class-level annotations to classAnnotations_ and field/method/parameter-level
				 * annotations directly onto the matching (already-built) Field/Method by index. */
				void parseAnnotationsDirectory(const Reader& r_, uint32_t offset_, std::vector<Annotation>& classAnnotations_,
				                               std::vector<std::unique_ptr<Field>>& fields_, std::vector<std::unique_ptr<Method>>& methods_) const;

				std::vector<uint8_t> _buffer;
				std::vector<std::string> _strings;
				std::vector<uint32_t> _typeStringIdx;
				std::vector<ProtoIdRaw> _protoIds;
				std::vector<FieldIdRaw> _fieldIds;
				std::vector<MethodIdRaw> _methodIds;
				std::vector<std::unique_ptr<Class>> _classes;
		};
	}  // namespace dex
}  // namespace sandvik

#endif  // __SANDVIK_DEX_FILE_HPP__
