#ifndef __METHOD_HPP__
#define __METHOD_HPP__

#include <cstdint>
#include <string>
#include <vector>

namespace LIEF {
	namespace DEX {
		class Method;
	}
}  // namespace LIEF

namespace sandvik {
	class Class;
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

			void debug() const;

		private:
			Class& _class;
			std::string _name;
			std::string _signature;
			uint32_t _index = 0;
			uint32_t _nbRegisters = 0;
			std::vector<uint8_t> _bytecode;

			bool _isPublic = false;
			bool _isProtected = false;
			bool _isPrivate = false;
			bool _isFinal = false;
			bool _isStatic = false;
			bool _isAbstract = false;
			bool _isNative = false;
			bool _isVirtual = false;

			struct trycatch_item {
					uint32_t start_addr;
					uint32_t insn_count;
					// holds type_idx, handler_offset
					std::vector<std::pair<uint32_t, uint32_t>> handlers;
					uint32_t catch_all_addr;
			};
			std::vector<trycatch_item> _trycatch_items;
	};
}  // namespace sandvik

#endif  // __METHOD_HPP__