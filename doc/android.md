# Build notes: Android 12 (AOSP) `core-libart.jar` and `core-oj.jar`

This document summarizes the steps used to build ART core libraries from AOSP Android 12.

## 1) Initialize and sync source

```bash
repo init -u https://android.googlesource.com/platform/manifest -b android-12.1.0_r27
repo sync -c -j12
```

## 2) Setup build environment

```bash
source build/envsetup.sh
lunch aosp_x86-eng
```

Selected target:

- `PLATFORM_VERSION=12`
- `TARGET_PRODUCT=aosp_x86`
- `TARGET_BUILD_VARIANT=eng`
- `BUILD_ID=SP2A.220505.008`

## 3) Build target jars

```bash
m core-oj core-libart
```

## 5) Result


Produced artifact:

```text
out/target/product/generic_x86/apex/com.android.art/javalib/core-oj.jar
out/target/product/generic_x86/apex/com.android.art/javalib/core-libart.jar
```
