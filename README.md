# Sandvik Project

![Sandvik Logo](doc/logo.png)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
![pipeline](https://gitlab.com/christophe.duvernois/sandvik/badges/main/pipeline.svg)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=christophe.duvernois_sandvik&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=christophe.duvernois_sandvik)

## Overview

The Sandvik project is a minimal Dalvik JVM written in C++.

It is a work in progress, aiming to become a full-featured Dalvik Virtual Machine (VM) implementation.

## Features

- Complete Dalvik Virtual Machine (VM) implementation.
- Support for Java Native Interface (JNI) for seamless integration with native code.
- Modular and extensible architecture to facilitate customization and scalability.
- Basic Java runtime support for executing essential Java functionalities.

## Getting Started

### Installation

1. Build the project:
	```bash
	./waf configure
	./waf build
	./waf install
	```

2. Run the tests:
	```bash
	./waf --tests
	```

3. Run the VM:
	```bash
	./sandvik
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
	Set the log level. Available levels: `NONE`, `DEBUG`, `INFO`, `WARN`, `ERROR`.

- `--logfile=[logfile]`
	Set the log output file.

- `--no-console`
	Disable console output.

- `-i, --instructions`
	Enable instruction trace (prints per-instruction VM execution details).

- `-c, --calltrace`
	Enable call trace (prints method entry/exit and call details).

- `--dex=[file]`
	Specify the DEX file to load.

- `--apk=[file]`
	Specify the APK file to load.

- `--jar=[file]`
	Specify the Jar files to load.

- `--main=[classname]`
	Specify the main class to run.

- `--runtime=[runtime]`
	Specify path to override the default Java runtime.

- `args...`
	Positional arguments for the Java program.

- `"--"`
	Can be used to terminate flag options and force all following arguments to be treated as positional options.

### Examples

To run the `HelloWorld` Java program:

1. Compile the Java source file:
	```bash
	javac HelloWorld.java
	```

2. Convert the compiled `.class` file to Dalvik bytecode using the `d8` tool:
	```bash
	d8 HelloWorld.class
	```

3. Run the generated `classes.dex` file with Sandvik:

This will execute the `HelloWorld` class from the specified DEX file, showcasing the VM's ability to interpret and run Dalvik bytecode. The output should display the expected behavior of the test program, such as printing "Hello, World!" to the console:
```bash
./wbuild/sandvik --log=NONE --jar android-12.0.0-bin/core-oj.dex.jar --jar android-12.0.0-bin/core-libart.dex.jar --jar android-12.0.0-bin/icu-stubs.dex.jar --dex tests/java/hello/classes.dex --main HelloWorld
[+]  === sandvik 1.0.0 ===
Hello, World!
```

Let's print instructions and calls :

```bash
./wbuild/sandvik -i -c --log=INFO --jar android-12.0.0-bin/core-oj.dex.jar --jar android-12.0.0-bin/core-libart.dex.jar --jar android-12.0.0-bin/icu-stubs.dex.jar --dex tests/java/hello/classes.dex --main HelloWorld

...
[*] 0002: const-string v0, string@1               : 1a 00 01 00                            HelloWorld::main([Ljava/lang/String;)V
[*] 0004: invoke-virtual {v1, v0}, meth@2         : 6e 20 02 00 01 00                      HelloWorld::main([Ljava/lang/String;)V
[*] invoke-virtual java.io.PrintStream.println(Ljava/lang/String;)V (this=java.io.PrintStream, "Hello, World!")
[*] 0000: monitor-enter v1                        : 1d 01                                  java.io.PrintStream::println(Ljava/lang/String;)V
[*] 0001: invoke-virtual {v1, v2}, meth@1590      : 6e 20 36 06 21 00                      java.io.PrintStream::println(Ljava/lang/String;)V
[*] invoke-virtual java.io.PrintStream.print(Ljava/lang/String;)V (this=java.io.PrintStream, "Hello, World!")
[*] 0000: if-nez v1, 0006 // +0004                : 39 01 04                               java.io.PrintStream::print(Ljava/lang/String;)V
[*] 0004: invoke-direct {v0, v1}, meth@1609       : 70 20 49 06 10 00                      java.io.PrintStream::print(Ljava/lang/String;)V
[*] invoke-direct java.io.PrintStream.write(Ljava/lang/String;)V (this=java.io.PrintStream, "Hello, World!")
[*] 0000: monitor-enter v2                        : 1d 02                                  java.io.PrintStream::write(Ljava/lang/String;)V
[*] 0001: invoke-direct {v2}, meth@1578           : 70 10 2a 06 02 00                      java.io.PrintStream::write(Ljava/lang/String;)V
[*] invoke-direct java.io.PrintStream.ensureOpen()V (this=java.io.PrintStream)
[*] 0000: iget-object v0, v2, type@700            : 54 20 bc 02                            java.io.PrintStream::ensureOpen()V
[*] 0002: if-eqz v0, 0005 // +0003                : 38 00 03                               java.io.PrintStream::ensureOpen()V
[*] 0004: return-void                             : 0e                                     java.io.PrintStream::ensureOpen()V
[*] 0004: invoke-direct {v2}, meth@1582           : 70 10 2e 06 02 00                      java.io.PrintStream::write(Ljava/lang/String;)V
[*] invoke-direct java.io.PrintStream.getTextOut()Ljava/io/BufferedWriter; (this=java.io.PrintStream)
...
```

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
