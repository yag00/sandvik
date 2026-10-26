# Sandvik Project

![Sandvik Logo](doc/logo.png)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
![pipeline](https://gitlab.com/christophe.duvernois/sandvik/badges/main/pipeline.svg)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=christophe.duvernois_sandvik&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=christophe.duvernois_sandvik)

## Overview

The Sandvik project is a minimal Dalvik VM written in C++.

It is a work in progress, aiming to become a full-featured Dalvik Virtual Machine (VM) implementation.

## Features

- Dalvik bytecode interpreter loading `.dex` files, DEX JARs and APKs, with its own DEX parser.
- Runs on the Android 12 (AOSP) core libraries (`core-oj`, `core-libart`, ICU, Conscrypt, BouncyCastle, ...).
- Java Native Interface (JNI) support, including loading native shared libraries.
- Native implementations of the Android runtime JNI methods (`android/12.0.0/natives`).
- Threads, monitors, garbage collector and reflection (including annotations).
- Virtual Android filesystem and system properties.
- Experimental Android zygote boot (`com.android.internal.os.ZygoteInit`).

## Getting Started

### Prerequisites

Linux x86_64 with:

- `clang` (default) or `gcc`, `cmake`, `autoconf`, `automake`, `libtool`, `texinfo`, `pkg-config`
- `python3` with `jinja2` and `requests`
- a JDK (`javac`, JDK 21 is used in CI)
- `git`, `curl`, `unzip`, `doxygen`, `graphviz`, `libicu-dev`

On Ubuntu 24.04:

```bash
sudo apt-get install -y curl unzip git doxygen graphviz clang gcc g++ pkg-config cmake python3 \
  python-is-python3 python3-jinja2 python3-requests autoconf automake libtool autotools-dev \
  texinfo openjdk-21-jdk libicu-dev
```

### Build

```bash
./waf configure
./waf build
./waf install
```

The first `./waf configure` takes a while: it fetches the git submodules (`fmt`, `libffi`, `args`, `googletest`, `axml-parser`, `xxHash`) and builds them into `ext/`, downloads the Android build-tools (for `d8`) into `ext/android/` and a `clang-format` binary into `ext/clang-format`.

Configure options:

- `--debug`: build with debug symbols (`-g`) and the `VERBOSE` / `TRACE` log levels.
- `--gcc`: build with gcc instead of clang.
- `--prefix=DIR`: installation directory (default: `delivery/`).

The build produces `wbuild/libsandvik.so` and `wbuild/sandvik`. Note that each build reformats the C++ sources with `clang-format`.

### Run the tests

```bash
./waf build --tests               # run all unit tests
./waf build --test-name=dex       # run only tests/unit/test_dex.cpp
```

A test report is generated in `wbuild/report.html`.

### Run the VM

`sandvik` must be launched from the repository root (the Android runtime is looked up in `android/`), with `libsandvik.so` on the library path:

```bash
LD_LIBRARY_PATH=wbuild ./wbuild/sandvik --help
```

## Usage

Sandvik can be used to run Dalvik bytecode files (`.dex`) or Android application packages (`.apk`). Below are the available options and their usage:

```bash
./sandvik {OPTIONS} [args...]
```

### Options

- `-h, --help`
  Display available options.

- `--version`
  Display the version of this program.

- `--log=[level]`
  Set the log level of the VM. Available levels, from the most verbose: `VERBOSE`, `TRACE`, `DEBUG`, `INFO` (default), `WARN`, `ERROR`, `NONE`. `VERBOSE` and `TRACE` are only available in a `--debug` build.

- `--app-log=[level]`
  Set the log level of the running Android application (`android.util.Log`, `System.log`), independently of `--log`. Available levels: `VERBOSE`, `DEBUG`, `INFO` (default), `WARN`, `ERROR`, `NONE`.

- `--logfile=[logfile]`
  Also write the logs to this file (without colors).

- `--no-console`
  Disable log output on the console.

- `-t, --display-thread`
  Display thread name in logs.

- `-i, --instructions`
  Enable instruction trace (prints per-instruction VM execution details).

- `-c, --calltrace`
  Enable call trace (prints method entry/exit and call details).

- `--dex=[file]`
  Specify a DEX file to load (can be repeated).

- `--apk=[file]`
  Specify the APK file to load. The main class is read from the APK manifest.

- `--android-version=[version]`
  Specify the Android version to emulate (must be the name of a folder inside the `./android/` directory, e.g. `12.0.0`). It sets the defaults for `--jar-dir` (`android/{version}/bin`), `--android-root` (`android/{version}/fs`) and `--prop` (`android/{version}/getprop.prop`).

- `--jar-dir=[dir]`
  Specify the directory containing the runtime JAR files to load. If an `order.txt` file is found in this folder, only the JARs listed in it are loaded, in the specified order.

- `--jar=[file]`
  Specify a runtime JAR file to load (can be repeated).

- `--android-root=[dir]`
  Specify the directory containing the virtual Android filesystem root.

- `--prop=[file]`
  Specify an Android `getprop` output file to preload system properties.

- `--main=[classname]`
  Specify the main class to run (required unless `--apk` is used).

- `args...`
  Positional arguments for the Java program.

- `"--"`
  Can be used to terminate flag options and force all following arguments to be treated as positional options.

### Logs

Logs are written to stderr, stdout only carries the output of the Java program. Each line starts with a marker telling what it is:

| Marker  | Meaning                                                                  |
| ------- | ------------------------------------------------------------------------ |
| `[!]`   | VM error                                                                 |
| `[w]`   | VM warning                                                               |
| `[+]`   | VM success message                                                       |
| `[*]`   | VM information: runtime, main class, run duration                        |
| `[d]`   | VM debug: class, jar and library loading, threads, GC                    |
| `[t]`   | VM trace: frames, native calls, method resolution, exceptions            |
| `[v]`   | VM verbose: registers (`v5 <- 31`), fields, new instances, monitors      |
| `[app]` | log of the running application, logcat style (`[app] W/Zygote: message`) |
| `[i]`   | instruction trace (`-i`)                                                 |
| `[c]`   | call trace (`-c`)                                                        |

On a terminal each kind of line has its own color (set `NO_COLOR` to disable them).

### Examples

To run the `HelloWorld` Java program:

1. Compile the Java source file:

   ```bash
   javac HelloWorld.java
   ```

2. Convert the compiled `.class` file to Dalvik bytecode using the `d8` tool (installed by `./waf configure`):

   ```bash
   ext/android/build-tools/36.0.0/d8 HelloWorld.class
   ```

3. Run the generated `classes.dex` file with Sandvik:

This will execute the `HelloWorld` class from the specified DEX file, showcasing the VM's ability to interpret and run Dalvik bytecode. The output should display the expected behavior of the test program, such as printing "Hello, World!" to the console:

```bash
LD_LIBRARY_PATH=wbuild ./wbuild/sandvik --android-version=12.0.0 --dex tests/java/hello/classes.dex --main HelloWorld
[+] === sandvik 1.0.0-xxxxxxx ===
[*] Runtime: Android 12.0.0 (android/12.0.0/bin)
[*] Running class: HelloWorld
Hello, World!
[*] === end (0.101s) ===
```

With `--log=NONE` only `Hello, World!` is printed.

The runtime JARs can also be given explicitly:

```bash
LD_LIBRARY_PATH=wbuild ./wbuild/sandvik --log=NONE --jar android/12.0.0/bin/core-oj.dex.jar --jar android/12.0.0/bin/core-libart.dex.jar --jar android/12.0.0/bin/icu-stubs.dex.jar --dex tests/java/hello/classes.dex --main HelloWorld
```

Let's print instructions and calls :

```bash
LD_LIBRARY_PATH=wbuild ./wbuild/sandvik -i -c --android-version=12.0.0 --dex tests/java/hello/classes.dex --main HelloWorld

...
[i] 0002: const-string v0, string@1               : 1a 00 01 00                            HelloWorld::main([Ljava/lang/String;)V
[i] 0004: invoke-virtual {v1, v0}, meth@2         : 6e 20 02 00 01 00                      HelloWorld::main([Ljava/lang/String;)V
[c] invoke-virtual java.io.PrintStream.println(this=java.io.PrintStream, String="Hello, World!")
[i] 0000: monitor-enter v1                        : 1d 01                                  java.io.PrintStream::println(Ljava/lang/String;)V
[i] 0001: invoke-virtual {v1, v2}, meth@1590      : 6e 20 36 06 21 00                      java.io.PrintStream::println(Ljava/lang/String;)V
[c] invoke-virtual java.io.PrintStream.print(this=java.io.PrintStream, String="Hello, World!")
[i] 0000: if-nez v1, 0006 // +0004                : 39 01 04                               java.io.PrintStream::print(Ljava/lang/String;)V
[i] 0004: invoke-direct {v0, v1}, meth@1609       : 70 20 49 06 10 00                      java.io.PrintStream::print(Ljava/lang/String;)V
[c] invoke-direct java.io.PrintStream.write(this=java.io.PrintStream, String="Hello, World!")
[i] 0000: monitor-enter v2                        : 1d 02                                  java.io.PrintStream::write(Ljava/lang/String;)V
[i] 0001: invoke-direct {v2}, meth@1578           : 70 10 2a 06 02 00                      java.io.PrintStream::write(Ljava/lang/String;)V
[c] invoke-direct java.io.PrintStream.ensureOpen(this=java.io.PrintStream)
[i] 0000: iget-object v0, v2, type@700            : 54 20 bc 02                            java.io.PrintStream::ensureOpen()V
...
```

The call trace shows each argument with its parameter type (`int=3`, `String="..."`, `boolean=false`) and the return type (`-> Object`). In a `--debug` build, `--log=VERBOSE` adds the register and field accesses under each instruction (`[v]   v0 <- "Hello, World!"`).

### Zygote (experimental)

Sandvik can start the Android 12 zygote (`ZygoteInit`) on top of the AOSP runtime:

```bash
LD_LIBRARY_PATH=wbuild ./wbuild/sandvik --jar-dir=android/12.0.0/bin --android-root=android/12.0.0/fs --main com.android.internal.os.ZygoteInit -- dummy --abi-list=x86_64
...
[app] I/Zygote: Preloading classes...
...
[app] I/Zygote: Accepting command socket connections
```

or simply:

```bash
./waf run
```

The zygote preloads the framework then waits for fork requests on its command socket: it does not exit by itself, stop it with `Ctrl-C`. This is a work in progress, many natives are still stubs.

## Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository.
2. Create a new branch for your feature or bugfix.
3. Submit a pull request with a detailed description.

## License

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

This project is licensed under the GNU General Public License v3.0. See the [LICENSE](LICENSE) file for details.

## Contact

For questions or support, please contact [me](mailto:christophe.duvernois+github@gmail.com).
