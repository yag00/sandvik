---
name: sandvik-build
description: Configure, build, install or clean the Sandvik Dalvik VM with waf (clang/gcc, debug mode, external deps in ext/, clang-format checkstyle). Use when asked to build/compile/rebuild Sandvik, set up a fresh checkout, switch compiler, or fix configure/link errors.
---

# Building Sandvik

Sandvik is a C++20 Dalvik VM built with **waf** (`./waf`, config in `wscript`, helpers in `wtools/`).
Always run waf from the repository root. Build output goes to `wbuild/`, install to `delivery/`.

## Commands

```bash
./waf configure            # clang by default; downloads/builds deps (see below)
./waf configure --debug    # adds -g and defines __debug__
./waf configure --gcc      # use gcc/g++ instead of clang
./waf build                # build libsandvik.so + sandvik executable
./waf install              # install to delivery/ (override with configure --prefix=DIR)
./waf clean                # remove build objects
./waf distclean            # remove wbuild/ and waf lock files (requires a new configure)
```

`--debug` and `--gcc` are **configure** options: changing them requires re-running `./waf configure`.
`./waf` with no command = `./waf build`.

## What configure does (first run is long)

- `git submodule init/update` for `ext/fmt`, `ext/libffi`, `ext/args`, `ext/googletest`, `ext/axml-parser`, `ext/xxHash`.
- Builds each local lib into `ext/<name>-bin/` (cmake/autotools) if not already present.
- Downloads Android command-line tools + build-tools 36.0.0 into `ext/android/` to get `d8` (`ext/android/build-tools/36.0.0/d8`).
- Downloads a static `clang-format` 21 into `ext/clang-format` if missing.
- Requires `javac` (JDK 21 in CI), python3 with `jinja2` and `requests`, `doxygen`, `libicu-dev` (ICU headers + `-licuuc`).

System packages used by CI (Ubuntu 24.04):

```bash
apt-get install -y curl unzip git doxygen graphviz clang gcc g++ pkg-config cmake python3 \
  python-is-python3 python3-jinja2 python3-requests autoconf automake libtool autotools-dev \
  texinfo openjdk-21-jdk libicu-dev
```

## Build outputs

- `wbuild/libsandvik.so` — the VM library: all of `src/**` (except `src/main.cpp`) **plus** `android/12.0.0/natives/**/*.cpp` (JNI natives of the Android runtime).
- `wbuild/sandvik` — CLI executable (`src/main.cpp`), linked with `-rdynamic`, **no rpath**: run it with `LD_LIBRARY_PATH=wbuild` (see the `sandvik-run` skill).
- `wbuild/compile_commands.json` — generated with clang (used by Sonar / clangd).
- `src/version.in.hpp` — generated from git info at each build.

New `.cpp` files under `src/` or `android/12.0.0/natives/` are picked up automatically by glob; no wscript change needed.

## Important behaviours

- **Every build runs clang-format in place** (`.clang-format`, `.clang-format-java`) on `src/**` and `android/12.0.0/natives/**`. Expect formatting diffs in files you touched; this is normal — don't fight the formatter, and don't hand-format against it.
- Flags include `-Wall -Werror -O3 -march=native`: any warning breaks the build. Fix warnings, don't silence them (unused params are already allowed; the codebase uses `(void)param;` for clarity).
- Link with `-Wl,-z,defs`: an undefined symbol in libsandvik fails at link time, not at runtime.

## Troubleshooting

- Missing `ext/*` content / submodule errors: `git submodule update --init`, then `./waf configure`.
- A dependency in `ext/<lib>-bin` is broken: delete that `-bin` dir and re-run `./waf configure` to rebuild it.
- Changed compiler or debug flag but nothing happened: you forgot to re-run `./waf configure`.
- Weird stale state: `./waf distclean && ./waf configure && ./waf build`.

To run the test suite, see the `sandvik-test` skill.
