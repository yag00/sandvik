#ifndef __APK_LOADER_HPP__
#define __APK_LOADER_HPP__

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace sandvik {
	class Class;
	class Dex;
	class ZipReader;
	class Apk {
		public:
			Apk() = default;
			explicit Apk(const std::string& path);
			~Apk();

			// Prevent copying
			Apk(const Apk&) = delete;
			Apk& operator=(const Apk&) = delete;

			// Allow moving
			Apk(Apk&&) noexcept;
			Apk& operator=(Apk&&) noexcept;

			std::string getPath() const;

			void load(const std::string& path);

			std::vector<std::string> getClassNames() const;
			std::string getMainActivity() const;
			std::unique_ptr<Class> findClass(const std::string& name) const;

		protected:
			std::string findMainActivity() const;

		private:
			std::string _path;
			std::unique_ptr<ZipReader> _zipReader;
			std::unique_ptr<Dex> _classes_dex;
			std::string _manifest;
			std::string _mainActivity;
	};
}  // namespace sandvik
#endif  // __APK_LOADER_HPP__