#ifndef __DISASSEMBLER_HPP__
#define __DISASSEMBLER_HPP__

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace sandvik {
	class Disassembler {
		public:
			Disassembler();
			~Disassembler() = default;

			std::string disassemble(const uint8_t opcode_);
			std::string disassemble(const uint8_t* bytecode_, uint32_t& size_);
			std::string disassemble(const uint8_t* bytecode_);

		private:
			std::string format_i10x(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i12x(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i11n(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i11x(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i10t(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i20t(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i22x(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i21t(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i21s(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i21h(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i21c(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i23x(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i22b(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i22t(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i22s(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i22c(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i30t(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i32x(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i31i(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i31t(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i31c(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i35c(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i3rc(const std::string& name_, const uint8_t* operand_, uint32_t& size_);
			std::string format_i51l(const std::string& name_, const uint8_t* operand_, uint32_t& size_);

			std::vector<std::function<std::string(const std::string&, const uint8_t*, uint32_t&)>> _dispatch;
	};
}  // namespace sandvik

#endif  // __DISASSEMBLER_HPP__