#ifndef __DEX_LOADER_HPP__
#define __DEX_LOADER_HPP__

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace LIEF {
	namespace DEX {
		class File;
	}  // namespace DEX
}  // namespace LIEF

namespace sandvik {
	class Class;
	class Dex {
		public:
			// Custom exception class
			class DexLoaderException : public std::runtime_error {
				public:
					using std::runtime_error::runtime_error;
			};

			explicit Dex(const uint32_t idx_, const std::string& path_);
			Dex(const uint32_t idx_);
			Dex(const uint32_t idx_, std::vector<uint8_t>& buffer, const std::string& name = "");
			~Dex();

			// Prevent copying
			Dex(const Dex&) = delete;
			Dex& operator=(const Dex&) = delete;

			std::string getPath() const;

			void load(const std::string& path);
			void load(std::vector<uint8_t>& buffer, const std::string& name = "");

			bool is_loaded() const noexcept;
			std::vector<std::string> getClassNames() const;
			std::unique_ptr<Class> findClass(const std::string& name) const;

			void resolveMethod(uint16_t idx, std::string& class_, std::string& method_, std::string& sig_);
			void resolveClass(uint16_t idx, std::string& class_);
			void resolveField(uint16_t idx, std::string& class_, std::string& field_);
			std::string resolveString(uint16_t idx);
			std::vector<std::pair<std::string, uint32_t>> resolveArray(uint16_t idx);

		private:
			const uint32_t _idx;
			std::string _path;
			std::unique_ptr<const LIEF::DEX::File> _dex;
	};
}  // namespace sandvik
#endif  // __DEX_LOADER_HPP__