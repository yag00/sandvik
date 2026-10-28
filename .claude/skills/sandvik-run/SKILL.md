---
name: sandvik-run
description: Run the Sandvik VM on a DEX/JAR/APK or launch the Android zygote (ZygoteInit), with instruction/call traces and logging to debug VM behaviour. Use when asked to run/launch Sandvik, run HelloWorld or a Java program, start zygote, compile Java to DEX, or trace/debug execution.
---

# Running Sandvik

Prerequisite: a successful build (`sandvik-build` skill). Two rules always apply:

1. **Run from the repository root** — `--android-version` resolves `android/<version>/...` relative to the CWD.
2. **Set `LD_LIBRARY_PATH=wbuild`** — `wbuild/sandvik` has no rpath to `libsandvik.so` (otherwise: `error while loading shared libraries: libsandvik.so`).

```bash
LD_LIBRARY_PATH=wbuild ./wbuild/sandvik [OPTIONS] [--] [java args...]
```

## Options (`src/main.cpp`)

| Option                                      | Meaning                                                                                                                           |
| ------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| `--android-version=12.0.0`                  | Shortcut: jars from `android/12.0.0/bin` (in `order.txt` order), FS root `android/12.0.0/fs`, props `android/12.0.0/getprop.prop` |
| `--jar-dir=DIR` / `--jar=FILE` (repeatable) | Runtime DEX jars (dir honours `order.txt`)                                                                                        |
| `--dex=FILE` (repeatable)                   | Application DEX file(s)                                                                                                           |
| `--apk=FILE`                                | Load an APK; main class comes from its manifest                                                                                   |
| `--android-root=DIR`                        | Virtual Android filesystem root                                                                                                   |
| `--prop=FILE`                               | getprop output file to preload system properties                                                                                  |
| `--main=CLASS`                              | Main class (required unless `--apk`)                                                                                              |
| `--log=LEVEL`                               | VM log level: `VERBOSE`, `TRACE`, `DEBUG`, `INFO` (default), `WARN`, `ERROR`, `NONE`. VERBOSE/TRACE need a `--debug` build        |
| `--app-log=LEVEL`                           | Android app log level (`android.util.Log`, `System.log`): `VERBOSE`, `DEBUG`, `INFO` (default), `WARN`, `ERROR`, `NONE`           |
| `--logfile=FILE`, `--no-console`            | Also log to a file (never colored) / silence console logging                                                                      |
| `-i`, `--instructions`                      | Per-instruction trace (very verbose)                                                                                              |
| `-c`, `--calltrace`                         | Method call trace with arguments                                                                                                  |
| `-t`, `--display-thread`                    | Prefix logs with thread name                                                                                                      |
| `--`                                        | Everything after is passed to the Java `main`                                                                                     |

Logs go to **stderr**; stdout only carries the Java program output (`2>/dev/null` keeps just the program output).
Markers: `[!]` error, `[w]` warning, `[+]`/`[*]` info, `[d]` debug, `[t]` trace, `[v]` verbose (indented under `[i]`),
`[app]` app logs in logcat style (`[app] W/Zygote: ...`), `[i]` instruction trace, `[c]` call trace. Colors only on a tty (`NO_COLOR=1` disables them).

Runtime layout: `android/12.0.0/bin/*.dex.jar` (AOSP core-oj, core-libart, icu, conscrypt, bouncycastle...), `android/12.0.0/fs/` (fake `/system`, `/etc`...), `android/12.0.0/natives/` (C++ JNI natives compiled into libsandvik).

## Hello World

```bash
LD_LIBRARY_PATH=wbuild ./wbuild/sandvik --log=NONE --android-version=12.0.0 \
  --dex tests/java/hello/classes.dex --main HelloWorld
```

Compile your own program to DEX (d8 is installed by `./waf configure`):

```bash
javac -d /tmp/out Foo.java
ext/android/build-tools/36.0.0/d8 --output /tmp/out /tmp/out/*.class   # -> /tmp/out/classes.dex
LD_LIBRARY_PATH=wbuild ./wbuild/sandvik --android-version=12.0.0 --dex /tmp/out/classes.dex --main Foo -- arg1
```

## APK

`--apk tests/java/helloapk/hello.apk` runs `android.app.SandvikLauncher`, which plays `system_server` in-process and launches the manifest launcher activity through `ActivityThread` (prints `Hello World!` from `onCreate`). Rebuild the test APK with `tests/java/helloapk/build.sh`, the launcher/services jar with `android/12.0.0/java/build.sh`.
Mechanics and debugging: `doc/app-runtime.md`.

## Zygote

Launches `com.android.internal.os.ZygoteInit` on the Android 12 runtime. Sandvik creates a stub
`ANDROID_SOCKET_zygote` Unix socket (`src/zygote.cpp`) so ZygoteInit's startup checks pass.

```bash
LD_LIBRARY_PATH=wbuild ./wbuild/sandvik -c --log=INFO --android-version=12.0.0 \
  --main com.android.internal.os.ZygoteInit -- dummy --abi-list=x86_64
```

Equivalent short forms:

- `LD_LIBRARY_PATH=wbuild ./wbuild/sandvik --android-version=12.0.0 --main com.android.internal.os.ZygoteInit -- dummy --abi-list=x86_64`
- `./waf run` — same thing with `--log=ERROR` (args hard-coded in `run()` of `wscript`).

The zygote preloads the framework (~1-2 min), logs `[app] I/Zygote: Accepting command socket connections` and then
**waits forever** in `accept` on its command socket: always run it with `timeout -s INT <seconds>` (or stop it with Ctrl-C).
A leftover zygote keeps the abstract socket bound and the next run fails with `bind() failed: Address already in use`:
check with `pgrep -ax sandvik` and kill it. Most `[w] ... not implemented` / `TODO: lib*.so is not supported` warnings are known stubs.

## Debugging workflow

Traces are huge (the zygote produces millions of lines): always send them to a file and work from its tail.

```bash
LD_LIBRARY_PATH=wbuild timeout -s INT 300 ./wbuild/sandvik -c --log=DEBUG --logfile=/tmp/sandvik.log --no-console \
  --android-version=12.0.0 --main com.android.internal.os.ZygoteInit -- dummy --abi-list=x86_64
tail -200 /tmp/sandvik.log
grep '^\[c\]' /tmp/sandvik.log | tail -50          # filter by marker
```

- Start with `-c` (calls: typed arguments `int=3, String="..."` and `-> ReturnType`) to locate the failing method; add `-i` only when narrowed down.
- `--log=TRACE` (frames, native calls, exceptions) and `--log=VERBOSE` (register/field accesses `v5 <- 31`, `iget X.f -> ...`, objects as `Class@hash`) need `./waf configure --debug && ./waf build`; reconfigure without `--debug` afterwards. In a release build they print a warning instead.
- `--app-log=DEBUG` shows the app `Log.d` messages (e.g. `Zygote64Timing`).
- Wrap long runs with `timeout` — a hang in a native or a monitor otherwise blocks forever.
- `Native method Java_... is not available!` → a JNI native is missing: see the `sandvik-native` skill.
- For native crashes: `LD_LIBRARY_PATH=wbuild gdb --args ./wbuild/sandvik ...` (configure with `--debug` for symbols).
