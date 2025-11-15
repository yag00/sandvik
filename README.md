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
./wbuild/sandvik -i -c --log=INFO --dex tests/java/hello/classes.dex --main=HelloWorld
[+]  === sandvik 1.0.0-983807d ===
[*] VM instance created.
[*] Running class: HelloWorld
[*] 0000: sget-object v1, string@0                : 62 01 00 00                            HelloWorld::main([Ljava/lang/String;)V
[java.lang.System.<clinit>] [*] 0000: const/4 v0, #0                          : 12 00                                  java.lang.System::<clinit>()V
[java.lang.System.<clinit>] [*] 0001: sput-object v0, string@1                : 69 00 01 00                            java.lang.System::<clinit>()V
[java.lang.System.<clinit>] [*] 0003: sput-object v0, string@2                : 69 00 02 00                            java.lang.System::<clinit>()V
[java.lang.System.<clinit>] [*] 0005: sput-object v0, string@0                : 69 00 00 00                            java.lang.System::<clinit>()V
[java.lang.System.<clinit>] [*] 0007: return-void                             : 0e                                     java.lang.System::<clinit>()V
[java.lang.System.<clinit>] [*] 0000: invoke-static method@9                  : 71 00 09 00 00 00                      java.lang.System::initializeSystemClass()V
[java.lang.System.<clinit>] [*] invoke-static java.lang.System.initializeStream()V ()
[java.lang.System.<clinit>] [*] 0003: return-void                             : 0e                                     java.lang.System::initializeSystemClass()V
[*] 0002: const-string v0, string@1               : 1a 00 01 00                            HelloWorld::main([Ljava/lang/String;)V
[*] 0004: invoke-virtual {v1, v0}, method@2       : 6e 20 02 00 01 00                      HelloWorld::main([Ljava/lang/String;)V
[*] invoke-virtual java.io.PrintStream.println(Ljava/lang/String;)V (this=Instance of java.io.PrintStream, String=Hello, World!)
Hello, World!
[*] 0007: return-void                             : 0e                                     HelloWorld::main([Ljava/lang/String;)V
[*]  === end ===
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
