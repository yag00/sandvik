#!/bin/sh
# Build ../bin/sandvik-framework.dex.jar: framework classes provided by Sandvik (app launcher, replacements of
# framework stubs), compiled against the Android 12 framework classes (hidden API included).
# It is listed before javalib.dex.jar in order.txt, so its classes take precedence.
set -e
cd "$(dirname "$0")"
ROOT=../../..
BT=$ROOT/ext/android/build-tools/36.0.0
FRAMEWORK=../bin/javalib.jar:../bin/core-libart.jar:../bin/core-oj.jar
OUT=build
rm -rf $OUT && mkdir -p $OUT/classes $OUT/gen
python3 gen_service_bases.py ../bin/javalib.jar $OUT/gen
javac --release 11 -nowarn -cp $FRAMEWORK -d $OUT/classes $(find src $OUT/gen -name '*.java')
$BT/d8 --min-api 31 --classpath ../bin/javalib.jar --output ../bin/sandvik-framework.dex.jar $(find $OUT/classes -name '*.class')
rm -rf $OUT
