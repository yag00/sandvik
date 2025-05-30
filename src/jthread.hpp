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
			uint64_t stackDepth() const;

			void execute();

		private:
			Vm& _vm;
			ClassLoader& _classloader;
			std::string _name;
			std::unique_ptr<Interpreter> _interpreter;

			std::vector<std::unique_ptr<Frame>> _stack;
	};
}  // namespace sandvik

#endif  // __JAVA_THREAD_HPP__