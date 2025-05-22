#ifndef __JVM_HPP__
#define __JVM_HPP__

#include <memory>
#include <string>
#include <vector>

namespace sandvik {
	class Class;
	class ClassLoader;
	class SharedLibrary;
	class NativeInterface;
	class Vm {
		public:
			explicit Vm();
			~Vm() = default;

			void loadDex(const std::string& path);
			void loadApk(const std::string& path);
			void run();
			void run(const std::string& mainClass_, const std::vector<std::string>& args_);
			void stop();

			void loadLibrary(const std::string& libName_);
			void* findNativeSymbol(const std::string& symbolName_);
			NativeInterface* getJNIEnv() const;

		protected:
			void run(Class& clazz_, const std::vector<std::string>& args_);

		private:
			std::unique_ptr<ClassLoader> _classloader;
			std::vector<std::string> _ldpath;
			std::vector<std::unique_ptr<SharedLibrary>> _sharedlibs;
			std::unique_ptr<NativeInterface> _jnienv;
	};
}  // namespace sandvik

#endif  // __JVM_HPP__