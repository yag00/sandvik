---
name: sandvik-test
description: Run, filter, debug and add Sandvik unit tests (googletest via waf, Java/DEX test programs in tests/java). Use when asked to run the tests, run a single test, check a change didn't break anything, investigate a failing test, or write a new test.
---

# Testing Sandvik

Tests are googletest programs in `tests/unit/test_*.cpp`, built and run by waf (`wtools/test.py`, `wtools/testrunner.py`).

## Run tests

```bash
./waf build --tests                 # build + run every tests/unit/test_*.cpp (same as ./waf --tests)
./waf build --test-name=dex         # only tests/unit/test_dex.cpp (name without "test_" and ".cpp")
./waf build --tests -v              # verbose: show gtest output of each binary
```

- Each `test_X.cpp` becomes its own binary `wbuild/test_X`, plus `wbuild/test_all` with everything.
- Results: per-file JSON `wbuild/report_test_X.json`, summary printed at the end (`PASS`/`FAIL` per test), HTML report in `wbuild/report.html`. Failures make the waf command fail.
- Default per-binary timeout: 300 s.
- CI runs `./waf build --tests` with both clang and gcc (`./waf configure --gcc`).

## Run a single gtest case directly (faster iteration)

Test binaries must run **from `wbuild/`** (runtime jars are loaded as `../android/12.0.0/bin/...`, test DEX as `../tests/java/...`) and need the native test libs on the library path:

```bash
./waf build --test-name=java        # build (and run) it once
cd wbuild && LD_LIBRARY_PATH=.:../tests/java/native:../tests/java/jni ./test_java --gtest_filter='JVM.Enums'
```

Use `--gtest_filter='Suite.*'`, `--gtest_list_tests`, etc. Run under `gdb --args` the same way for crashes.
Java tests redirect stdout to `wbuild/test_<Class>.out`, so the console output stops at `[ RUN ]`: check the exit code and that file instead.

## Test files

| File                                                                                   | What it covers                                                                                                                |
| -------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------- |
| `test_java.cpp`                                                                        | Java programs from `tests/java/unit/src/*.java` packaged in `tests/java/unit/TestUnitDex.jar`; each must print exactly `ok\n` |
| `test_dalvik.cpp`, `test_helloworld/fibonacci/add/native.ref`                          | DEX programs in `tests/java/{dalvik,hello,fib,add,native}` compared to `.ref` expected output                                 |
| `test_jni.cpp`                                                                         | JNI API, with `tests/java/jni` (`classes.dex` + `libtestjni.so`)                                                              |
| `test_dex.cpp`                                                                         | Sandvik's own DEX parser (`src/loader/dex*`), incl. annotations (`tests/java/annotations`)                                    |
| `test_object.cpp`, `test_gc.cpp`, `test_thread.cpp`, `test_stream.cpp`, `test_zip.cpp` | VM internals                                                                                                                  |

`tests/unit/common.hpp` → `initializeVmRuntime(vm)` loads the `core-oj`, `core-libart` and `icu-stubs` jars.

## Adding a Java behaviour test (most common)

1. Create `tests/java/unit/src/TestFoo.java` with a `main` that prints `ok` on success and `fail: <reason>` otherwise (copy the style of `TestEnums.java`). Optionally register it in `RunAllTests.java`.
2. Rebuild the DEX jar (needs `./waf configure` done, for `d8`):
   ```bash
   cd tests/java/unit && ./buildJar.sh
   ```
   It compiles with `javac`, checks it on the host JVM (`java -jar`), then produces `TestUnitDex.jar` with d8.
3. Add to `tests/unit/test_java.cpp`:
   ```cpp
   TEST(JVM, Foo) {
   	run_common_test("TestFoo");
   }
   ```
4. `./waf build --test-name=java`. Commit the `.java` **and** the regenerated `TestUnitDex.jar`.

Other DEX fixtures are rebuilt with `javac` + `ext/android/build-tools/36.0.0/d8 --output <dir> <classes>` (see `tests/java/jni/README.md` for the JNI fixture and its native lib).

## Debugging a failing Java test

Reproduce outside gtest with the CLI and traces (see `sandvik-run` skill):

```bash
LD_LIBRARY_PATH=wbuild ./wbuild/sandvik --log=DEBUG -c --android-version=12.0.0 \
  --jar tests/java/unit/TestUnitDex.jar --main TestFoo
```

The captured stdout of a gtest Java test is in `wbuild/test_TestFoo.out`.
