# Step 1: Compile all .java files into a build directory
javac -d build src/*.java
echo "Main-Class: RunAllTests" > manifest.txt
# Step 2: Package them into a jar
jar cfm TestUnit.jar manifest.txt -C build .
java -jar TestUnit.jar
../../../ext/android/build-tools/36.0.0/d8 --file-per-class-file --output TestUnitDex.jar TestUnit.jar
rm -rf build
rm manifest.txt TestUnit.jar