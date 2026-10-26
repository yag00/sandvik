---
name: sandvik-native
description: Implement or fix a JNI native method of the Android runtime in Sandvik (android/12.0.0/natives). Use when the VM fails with "Native method Java_... is not available!", when a native returns a wrong value, or when asked to add/stub a native for a Java class.
---

# Adding a native method

Java `native` methods of the Android runtime classes are implemented in C++ under `android/12.0.0/natives/<package path>/<Class>.cpp` and compiled **into libsandvik.so** (wscript glob, no registration table). At call time `Interpreter::executeNativeMethod` (`src/interpreter.cpp`) builds the JNI symbol name and resolves it with `dlsym` via `vm.findNativeSymbol`.

## Symbol name

- `Java_` + fully-qualified class with `.` → `_` + `_` + method name. e.g. `android.util.Log.isLoggable` → `Java_android_util_Log_isLoggable`.
- Overloaded methods add `__` + mangled signature (`JNIHelper::mangleMethodSignature`), standard JNI mangling: `_1` for `_`, `_2` for `;`, `_3` for `[`, `_0xxxx` for non-ASCII; e.g. `Java_java_lang_Foo_bar__ILjava_lang_String_2`. Inner classes: `$` → `_00024`.
- The exact name the VM looks for is printed in the error, or with `--log=TRACE` in a `--debug` build (`[t] call native jni function ...`). Copy it from there rather than mangling by hand.

## Implementation pattern

Follow the existing files (e.g. `android/12.0.0/natives/android/util/Log.cpp`):

```cpp
/* GPLv3 header copied from a neighbouring file */
#include <fmt/format.h>

#include "native_utils.hpp"
#include "system/logger.hpp"
// + "array.hpp", "class.hpp", "classloader.hpp", "exceptions.hpp", "object.hpp" as needed

using namespace sandvik;

extern "C" {
	JNIEXPORT jint JNICALL Java_pkg_Class_method(JNIEnv* env, jclass clazz, jstring name) {
		(void)env;
		(void)clazz;
		auto sName = sandvik::native::getString(name);   // helpers in src/native_utils.hpp
		logger.ftrace("Class.method called with {}", sName->str());
		return 0;
	}
}  // extern "C"
```

- Static methods get `jclass`, instance methods get `jobject` (this) as 2nd parameter.
- Mark unused parameters with `(void)x;` — `-Werror` is on.
- Look in `src/native_utils.hpp` / `src/jnihelper.hpp` for helpers (strings, arrays) before writing new conversions. For null object refs use `Object::makeNull()` (`src/object.hpp`) rather than `nullptr`.
- Add a method to the existing `<Class>.cpp` if it exists; otherwise create the file in the matching package dir.
- Log with `logger.f*` (they skip formatting when the level is off): `logger.ftrace` for a native called repeatedly, `logger.fdebug` for a one-shot stub, `logger.fwarning` for a not implemented native; return a sensible default, as `SystemProperties` does.
- Look at the AOSP Java source (`libcore`, `frameworks/base`) for the native's contract before stubbing.

## Verify

```bash
./waf build                         # also reformats the new file with clang-format
LD_LIBRARY_PATH=wbuild ./wbuild/sandvik -c ... 2>&1 | grep 'Class.method'        # the call trace shows the native being called
./waf build --tests                 # make sure nothing else regressed
```
