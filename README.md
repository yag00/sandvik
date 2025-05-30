# Sandvik Project

![Sandvik Logo](logo.png)

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

1. Clone the repository:
	```bash
	git clone https://github.com/yourusername/sandvik.git
	cd sandvik
	```

2. Build the project:
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

- `--dex=[file]`
	Specify the DEX file to load.

- `--apk=[file]`
	Specify the APK file to load.

- `--main=[classname]`
	Specify the main class to run.

- `--runtime=[runtime]`
	Specify the runtime path.

- `args...`
	Positional arguments for the Java program.

- `"--"`
	Can be used to terminate flag options and force all following arguments to be treated as positional options.

### Examples

```bash
./wbuild/sandvik --dex tests/java/dalvik/classes.dex --main DalvikTest
```

## Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository.
2. Create a new branch for your feature or bugfix.
3. Submit a pull request with a detailed description.

## License

This project is licensed under the GNU General Public License v3.0. See the [LICENSE](LICENSE) file for details.

## Contact

For questions or support, please contact [me](mailto:christophe.duvernois+github@gmail.com).