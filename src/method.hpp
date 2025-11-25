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

#ifndef __METHOD_HPP__
#define __METHOD_HPP__

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "object.hpp"

namespace LIEF {
	namespace DEX {
		class Method;
	}
}  // namespace LIEF

namespace sandvik {
	class Frame;
	class Class;
	/** @brief Access flags for methods. */
	enum ACCESS_FLAGS {
		ACC_UNKNOWN = 0x0,
		ACC_PUBLIC = 0x1,
		ACC_PRIVATE = 0x2,
		ACC_PROTECTED = 0x4,
		ACC_STATIC = 0x8,
		ACC_FINAL = 0x10,
		ACC_SYNCHRONIZED = 0x20,
		ACC_VOLATILE = 0x40,
		ACC_BRIDGE = 0x40,
		ACC_TRANSIENT = 0x80,
		ACC_VARARGS = 0x80,
		ACC_NATIVE = 0x100,
		ACC_INTERFACE = 0x200,
		ACC_ABSTRACT = 0x400,
		ACC_STRICT = 0x800,
		ACC_SYNTHETIC = 0x1000,
		ACC_ANNOTATION = 0x2000,
		ACC_ENUM = 0x4000,
		ACC_CONSTRUCTOR = 0x10000,
		ACC_DECLARED_SYNCHRONIZED = 0x20000
	};
	/** @brief Represents a method in a Java class. */
	class Method {
		public:
			/** Constructor for Method.
			 * @param class_ Reference to the Class object.
			 * @param name_ Name of the method.
			 * @param signature_ Signature of the method.
			 * @param index_ Index of the method.
			 */
			Method(Class& class_, const std::string& name_, const std::string& signature_, uint32_t index_);
			/** Constructor for Method from LIEF DEX Method.
			 * @param class_ Reference to the Class object.
			 * @param method_ Reference to the LIEF DEX Method object.
			 */
			Method(Class& class_, const LIEF::DEX::Method& method_);
			virtual ~Method() = default;

			/** @brief Gets the class of the method.
			 * @return Reference to the Class object.
			 */
			Class& getClass() const;
			/** @brief Gets the name of the method.
			 * @return Name of the method.
			 */
			std::string getName() const;
			/** @brief Gets the signature of the method.
			 * @return Signature of the method.
			 */
			std::string getSignature() const;
			/** @brief Gets the number of arguments of the method.
			 * @return Number of arguments.
			 */
			uint32_t getNbArguments() const;
			/** @brief Gets the type of the argument at the specified index.
			 * @param index_ Index of the argument.
			 * @return Type of the argument as a string.
			 */
			std::string getArgumentType(uint32_t index_) const;
			/** @brief Gets the list of argument types.
			 * @return Vector of argument types as strings.
			 */
			inline const std::vector<std::string>& arguments() const {
				return _argsType;
			}

			/** @brief Gets the number of registers used by the method.
			 * @return Number of registers.
			 */
			uint32_t getNbRegisters() const;
			/** @brief Gets the index of the method.
			 * @return Index of the method.
			 */
			uint32_t getIndex() const;
			/** @brief Gets the exception handler for the specified program counter.
			 * @param pc_ Program counter.
			 * @param catchAllAddr_ Reference to store the catch-all address.
			 * @return Vector of pairs representing the exception handlers (type_idx, handler_offset).
			 */
			std::vector<std::pair<uint32_t, uint32_t>> getExceptionHandler(uint16_t pc_, uint32_t& catchAllAddr_) const;

			/** @brief Checks if the method has bytecode.
			 * @return True if the method has bytecode, false otherwise.
			 */
			bool hasBytecode() const;
			/** @brief Gets the size of the bytecode.
			 * @return Size of the bytecode.
			 */
			uint32_t getBytecodeSize() const;
			/** @brief Gets the bytecode.
			 * @return Pointer to the bytecode.
			 */
			const uint8_t* const getBytecode() const;

			/** @brief Checks if the method is a static initializer.
			 * @return True if the method is a static initializer, false otherwise.
			 */
			bool isStaticInitializer() const;
			/** @brief Checks if the method is a constructor.
			 * @return True if the method is a constructor, false otherwise.
			 */
			bool isConstructor() const;
			/** @brief Checks if the method is public.
			 * @return True if the method is public, false otherwise.
			 */
			bool isPublic() const;
			/** @brief Checks if the method is private.
			 * @return True if the method is private, false otherwise.
			 */
			bool isPrivate() const;
			/** @brief Checks if the method is protected.
			 * @return True if the method is protected, false otherwise.
			 */
			bool isProtected() const;
			/** @brief Checks if the method is static.
			 * @return True if the method is static, false otherwise.
			 */
			bool isStatic() const;
			/** @brief Checks if the method is final.
			 * @return True if the method is final, false otherwise.
			 */
			bool isFinal() const;
			/** @brief Checks if the method is abstract.
			 * @return True if the method is abstract, false otherwise.
			 */
			bool isAbstract() const;
			/** @brief Checks if the method is native.
			 * @return True if the method is native, false otherwise.
			 */
			bool isNative() const;
			/** @brief Checks if the method is virtual.
			 * @return True if the method is virtual, false otherwise.
			 */
			bool isVirtual() const;
			/** @brief Checks if the method is overloaded.
			 * @return True if the method is overloaded, false otherwise.
			 */
			bool isOverload() const;

			/** @brief Executes the method.
			 * @param frame_ Reference to the Frame object.
			 * @param registers_ Vector of Object references representing the registers.
			 */
			void execute(Frame& frame_, std::vector<ObjectRef>& registers_);

			/** @brief Debug method to print method information. */
			void debug() const;

		private:
			void parseArgumentTypes();

			Class& _class;
			std::string _name;
			std::string _signature;
			uint32_t _index;
			uint32_t _nbRegisters = 0;
			std::vector<uint8_t> _bytecode;
			uint64_t _accessFlags = 0;
			bool _isVirtual = false;

			struct trycatch_item {
					uint32_t start_addr;
					uint32_t insn_count;
					// holds type_idx, handler_offset
					std::vector<std::pair<uint32_t, uint32_t>> handlers;
					uint32_t catch_all_addr;
			};
			std::vector<trycatch_item> _trycatch_items;
			std::vector<std::string> _argsType;

			std::function<void(Frame&, std::vector<ObjectRef>&)> _function;

			friend class ClassBuilder;
	};
}  // namespace sandvik

#endif  // __METHOD_HPP__