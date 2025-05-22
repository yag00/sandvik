#ifndef __JAVA_THREAD_HPP__
#define __JAVA_THREAD_HPP__

#include <memory>
#include <string>
#include <vector>

namespace sandvik {
	class Vm;
	class Object;
	class Frame;
	class Method;
	class Interpreter;
	class ClassLoader;
	class JThread {
		public:
			explicit JThread(Vm& vm_, ClassLoader& classloader_, const std::string& name_);
			~JThread() = default;

			Vm& vm() const;
			ClassLoader& getClassLoader() const;
			bool end() const;

			Frame& newFrame(Method& method_);
			void popFrame();
			Frame& currentFrame() const;

			void execute();

			bool handleConstructor(const std::string& class_, const std::string& method_, const std::string& sig_,
			                       std::vector<std::shared_ptr<Object>> args_ = {});
			bool handleInstanceMethod(Frame& frame_, const std::string& class_, const std::string& method_, const std::string& sig_,
			                          std::vector<std::shared_ptr<Object>> args_ = {});
			bool handleClassFieldGetter(const std::string& class_, const std::string& field_);

		private:
			Vm& _vm;
			ClassLoader& _classloader;
			std::string _name;
			std::unique_ptr<Interpreter> _interpreter;

			std::vector<std::unique_ptr<Frame>> _stack;
	};
}  // namespace sandvik

#endif  // __JAVA_THREAD_HPP__