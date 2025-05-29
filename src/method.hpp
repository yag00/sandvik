#ifndef __METHOD_HPP__
#define __METHOD_HPP__

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace LIEF {
	namespace DEX {
		class Method;
	}
}  // namespace LIEF

namespace sandvik {
	class Frame;
	class Object;
	class Class;
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
	class Method {
		public:
			Method(Class& class_, const std::string& name_, const std::string& signature_);
			Method(Class& class_, const LIEF::DEX::Method& method_);
			virtual ~Method() = default;

			Class& getClass() const;
			std::string getName() const;
			std::string getSignature() const;
			uint32_t getNbRegisters() const;
			uint32_t getIndex() const;
			std::vector<std::pair<uint32_t, uint32_t>> getExceptionHandler(uint16_t pc_, uint32_t& catchAllAddr_) const;

			bool hasBytecode() const;
			const uint8_t* const getBytecode() const;

			bool isStaticInitializer() const;
			bool isConstructor() const;
			bool isPublic() const;
			bool isPrivate() const;
			bool isProtected() const;
			bool isStatic() const;
			bool isFinal() const;
			bool isAbstract() const;
			bool isNative() const;
			bool isVirtual() const;

			void execute(Frame& frame_, std::vector<std::shared_ptr<Object>>& registers_);
			void debug() const;

		private:
			Class& _class;
			std::string _name;
			std::string _signature;
			uint32_t _index = 0;
			uint32_t _nbRegisters = 0;
			std::vector<uint8_t> _bytecode;
			uint64_t _accessFlags;
			bool _isVirtual;

			struct trycatch_item {
					uint32_t start_addr;
					uint32_t insn_count;
					// holds type_idx, handler_offset
					std::vector<std::pair<uint32_t, uint32_t>> handlers;
					uint32_t catch_all_addr;
			};
			std::vector<trycatch_item> _trycatch_items;

			std::function<void(Frame&, std::vector<std::shared_ptr<Object>>&)> _function;

			friend class ClassBuilder;
	};
}  // namespace sandvik

#endif  // __METHOD_HPP__