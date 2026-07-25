# Instructions

## Compile JAVA Test classes

```bash
javac -d classes *.java
```

## Compile `classes.dex` file

```bash
../../../ext/android/build-tools/36.0.0/d8 --output . classes/*.class
```

## Compile Native Test Lib

```bash
gcc -fPIC -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/linux" -Iincludes -shared -o libtestjni.so src/*.c
```
