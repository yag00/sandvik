#!/bin/sh
# Build hello.apk: a minimal Activity printing "Hello World!" in onCreate
set -e
cd "$(dirname "$0")"
ROOT=../../..
SDK=$ROOT/ext/android
BT=$SDK/build-tools/36.0.0
ANDROID_JAR=$SDK/platforms/android-31/android.jar
OUT=build
rm -rf $OUT && mkdir -p $OUT/classes
javac --release 11 -cp $ANDROID_JAR -d $OUT/classes $(find src -name '*.java')
$BT/d8 --min-api 31 --lib $ANDROID_JAR --output $OUT $(find $OUT/classes -name '*.class')
$BT/aapt2 link -o $OUT/unaligned.apk -I $ANDROID_JAR --manifest AndroidManifest.xml
(cd $OUT && zip -q unaligned.apk classes.dex)
$BT/zipalign -f 4 $OUT/unaligned.apk hello.apk
rm -rf $OUT
