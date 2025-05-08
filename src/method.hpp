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
			explicit Method(Class& class_, const LIEF::DEX::Method& method_);
			Method(const Method& other);
			~Method() = default;

			Class& getClass() const;
			std::string getName() const;
			std::string getSignature() const;
			uint32_t getNbRegisters() const;

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
			bool isVoid() const;
			bool isVirtual() const;

			void debug() const;

		private:
			Class& _class;
			const LIEF::DEX::Method& _method;
	};
}  // namespace sandvik

#endif  // __METHOD_HPP__