#ifndef __JAVA_BASE_CLASSES_HPP__
#define __JAVA_BASE_CLASSES_HPP__

#include <ostream>
#include <string>

namespace sandvik {
	class ClassLoader;
}

namespace java {
	namespace io {
		void PrintStream(::sandvik::ClassLoader& classLoader);
	}  // namespace io
	namespace lang {
		void Class(::sandvik::ClassLoader& classLoader);
		void Double(::sandvik::ClassLoader& classLoader);
		void IllegalArgumentException(::sandvik::ClassLoader& classLoader);
		void Object(::sandvik::ClassLoader& classLoader);
		void RuntimeException(::sandvik::ClassLoader& classLoader);
		void String(::sandvik::ClassLoader& classLoader);
		void StringBuilder(::sandvik::ClassLoader& classLoader);
		void System(::sandvik::ClassLoader& classLoader);
	}  // namespace lang
}  // namespace java

#endif  // __JAVA_BASE_CLASSES_HPP__