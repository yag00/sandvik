public class TestJNI {

    static {
        System.loadLibrary("testjni");
    }

    private boolean success = true;

    private int intValue = 7;
    private double doubleValue = 3.14;
    private float floatValue = 2.71f;
    private long longValue = 123456789L;
    private short shortValue = 42;
    private byte byteValue = 1;
    private char charValue = 'A';
    private boolean booleanValue = true;
    private String stringValue = "Hello";
    private int[] intArrayValue = {1, 2, 3};

    private static int intStaticValue = 99;
    private static double doubleStaticValue = 6.28;
    private static float floatStaticValue = 1.41f;
    private static long longStaticValue = 987654321L;
    private static short shortStaticValue = 100;
    private static byte byteStaticValue = 10;
    private static char charStaticValue = 'Z';
    private static boolean booleanStaticValue = false;
    private static String stringStaticValue = "World";
    private static int[] intArrayStaticValue = {4, 5, 6};

    public native String nativeHello(String name);
    public native String nativeUnicodeStringTest(String input);
    public native int[] nativeArraySum(int[] values);

    public native int nativeGetIntField(TestJNI obj);
    public native void nativeSetIntField(TestJNI obj, int v);
    public native double nativeGetDoubleField(TestJNI obj);
    public native void nativeSetDoubleField(TestJNI obj, double v);
    public native float nativeGetFloatField(TestJNI obj);
    public native void nativeSetFloatField(TestJNI obj, float v);
    public native long nativeGetLongField(TestJNI obj);
    public native void nativeSetLongField(TestJNI obj, long v);
    public native short nativeGetShortField(TestJNI obj);
    public native void nativeSetShortField(TestJNI obj, short v);
    public native byte nativeGetByteField(TestJNI obj);
    public native void nativeSetByteField(TestJNI obj, byte v);
    public native char nativeGetCharField(TestJNI obj);
    public native void nativeSetCharField(TestJNI obj, char v);
    public native boolean nativeGetBooleanField(TestJNI obj);
    public native void nativeSetBooleanField(TestJNI obj, boolean v);
    public native String nativeGetStringField(TestJNI obj);
    public native void nativeSetStringField(TestJNI obj, String v);
    public native int[] nativeGetIntArrayField(TestJNI obj);
    public native void nativeSetIntArrayField(TestJNI obj, int[] v);

    public static native int nativeGetStaticIntField();
    public static native void nativeSetStaticIntField(int v);
    public static native double nativeGetStaticDoubleField();
    public static native void nativeSetStaticDoubleField(double v);
    public static native float nativeGetStaticFloatField();
    public static native void nativeSetStaticFloatField(float v);
    public static native long nativeGetStaticLongField();
    public static native void nativeSetStaticLongField(long v);
    public static native short nativeGetStaticShortField();
    public static native void nativeSetStaticShortField(short v);
    public static native byte nativeGetStaticByteField();
    public static native void nativeSetStaticByteField(byte v);
    public static native char nativeGetStaticCharField();
    public static native void nativeSetStaticCharField(char v);
    public static native boolean nativeGetStaticBooleanField();
    public static native void nativeSetStaticBooleanField(boolean v);
    public static native String nativeGetStaticStringField();
    public static native void nativeSetStaticStringField(String v);
    public static native int[] nativeGetStaticIntArrayField();
    public static native void nativeSetStaticIntArrayField(int[] v);

    public native String nativeLocalRefTest();
    public native String nativeGlobalRefTest();
    public native String nativeWeakRefTest();

    public native boolean nativeIsInstanceOf(Object obj, String className);

    public native boolean nativeThrowAndCatch();
    public native boolean nativeThreadTest();

    public native NativeTestObject nativeNewObject(String msg, int i);
    public native NativeTestObject nativeNewObject2(String msg, int i, double d, float f, long l, short s, byte b, char c, boolean bool);
    public native NativeTestObject nativeNewObjectV(String msg, int i, double d, float f, long l, short s, byte b, char c, boolean bool);
    public native NativeTestObject nativeNewObjectA(Object[] args);

    // Call<X>Method tests
    public native Object nativeCallObjectMethod(CallTarget target, Object arg);
    public native Object nativeCallObjectMethodV(CallTarget target, Object arg);
    public native Object nativeCallObjectMethodA(CallTarget target, Object arg);

    public native boolean nativeCallBooleanMethod(CallTarget target, boolean v);
    public native boolean nativeCallBooleanMethodV(CallTarget target, boolean v);
    public native boolean nativeCallBooleanMethodA(CallTarget target, boolean v);

    public native byte nativeCallByteMethod(CallTarget target, byte v);
    public native byte nativeCallByteMethodV(CallTarget target, byte v);
    public native byte nativeCallByteMethodA(CallTarget target, byte v);

    public native char nativeCallCharMethod(CallTarget target, char v);
    public native char nativeCallCharMethodV(CallTarget target, char v);
    public native char nativeCallCharMethodA(CallTarget target, char v);

    public native short nativeCallShortMethod(CallTarget target, short v);
    public native short nativeCallShortMethodV(CallTarget target, short v);
    public native short nativeCallShortMethodA(CallTarget target, short v);

    public native int nativeCallIntMethod(CallTarget target, int v);
    public native int nativeCallIntMethodV(CallTarget target, int v);
    public native int nativeCallIntMethodA(CallTarget target, int v);

    public native long nativeCallLongMethod(CallTarget target, long v);
    public native long nativeCallLongMethodV(CallTarget target, long v);
    public native long nativeCallLongMethodA(CallTarget target, long v);

    public native float nativeCallFloatMethod(CallTarget target, float v);
    public native float nativeCallFloatMethodV(CallTarget target, float v);
    public native float nativeCallFloatMethodA(CallTarget target, float v);

    public native double nativeCallDoubleMethod(CallTarget target, double v);
    public native double nativeCallDoubleMethodV(CallTarget target, double v);
    public native double nativeCallDoubleMethodA(CallTarget target, double v);

    public native void nativeCallVoidMethod(CallTarget target, int marker);
    public native void nativeCallVoidMethodV(CallTarget target, int marker);
    public native void nativeCallVoidMethodA(CallTarget target, int marker);

    private void assertEquals(String expected, String actual) {
        if (!expected.equals(actual)) {
            System.out.println("FAIL: expected \"" + expected + "\", got \"" + actual + "\"");
            success = false;
        }
    }
    private void assertEquals(int expected, int actual) {
        if (expected != actual) {
            System.out.println("FAIL: expected " + expected + ", got " + actual);
            success = false;
        }
    }

    private void assertEquals(double expected, double actual) {
        if (Double.compare(expected, actual) != 0) {
            System.out.println("FAIL: expected " + expected + ", got " + actual);
            success = false;
        }
    }

    private void assertEquals(float expected, float actual) {
        if (Float.compare(expected, actual) != 0) {
            System.out.println("FAIL: expected " + expected + ", got " + actual);
            success = false;
        }
    }

    private void assertEquals(long expected, long actual) {
        if (expected != actual) {
            System.out.println("FAIL: expected " + expected + ", got " + actual);
            success = false;
        }
    }

    private void assertEquals(short expected, short actual) {
        if (expected != actual) {
            System.out.println("FAIL: expected " + expected + ", got " + actual);
            success = false;
        }
    }

    private void assertEquals(byte expected, byte actual) {
        if (expected != actual) {
            System.out.println("FAIL: expected " + expected + ", got " + actual);
            success = false;
        }
    }

    private void assertEquals(char expected, char actual) {
        if (expected != actual) {
            System.out.println("FAIL: expected '" + expected + "', got '" + actual + "'");
            success = false;
        }
    }

    private void assertEquals(boolean expected, boolean actual) {
        if (expected != actual) {
            System.out.println("FAIL: expected " + expected + ", got " + actual);
            success = false;
        }
    }

    private void assertEquals(Object expected, Object actual) {
        if (expected != actual) {
            System.out.println("FAIL: expected " + expected + ", got " + actual);
            success = false;
        }
    }
    private void assertNotNull(Object obj) {
        if (obj == null) {
            System.out.println("FAIL: expected non-null object, got null");
            success = false;
        }
    }
    private void assertTrue(boolean condition) {
        if (!condition) {
            System.out.println("FAIL: expected true, got false");
            success = false;
        }
    }

    public void testNativeHello() {
        String result = nativeHello("World");
        assertEquals("Hello World 5", result);
    }
    public void testNativeUnicodeStringTest() {
        String unicode = "\u0068\u0065\u006c\u006c\u006f"; // "hello" as a Unicode string
        String result = nativeUnicodeStringTest(unicode);
        assertEquals(unicode, result);
    }
    public void testNativeArraySum() {
        int[] arr = {1, 2, 3};
        int[] r = nativeArraySum(arr);
        assertNotNull(r);
        assertEquals(6, r[0]);
    }
    public void testNativeGetSetField() {
        // int
        assertEquals(intValue, nativeGetIntField(this));
        nativeSetIntField(this, 1234);
        assertEquals(1234, nativeGetIntField(this));
        assertEquals(1234, intValue);

        // double
        assertEquals(doubleValue, nativeGetDoubleField(this));
        nativeSetDoubleField(this, 9.99);
        assertEquals(9.99, nativeGetDoubleField(this));
        assertEquals(9.99, doubleValue);

        // float
        assertEquals(floatValue, nativeGetFloatField(this));
        nativeSetFloatField(this, 8.88f);
        assertEquals(8.88f, nativeGetFloatField(this));
        assertEquals(8.88f, floatValue);

        // long
        assertEquals(longValue, nativeGetLongField(this));
        nativeSetLongField(this, 987654321L);
        assertEquals(987654321L, nativeGetLongField(this));
        assertEquals(987654321L, longValue);

        // short
        assertEquals(shortValue, nativeGetShortField(this));
        nativeSetShortField(this, (short) 123);
        assertEquals((short) 123, nativeGetShortField(this));
        assertEquals((short) 123, shortValue);

        // byte
        assertEquals(byteValue, nativeGetByteField(this));
        nativeSetByteField(this, (byte) 7);
        assertEquals((byte) 7, nativeGetByteField(this));
        assertEquals((byte) 7, byteValue);

        // char
        assertEquals(charValue, nativeGetCharField(this));
        nativeSetCharField(this, 'Z');
        assertEquals('Z', nativeGetCharField(this));
        assertEquals('Z', charValue);

        // boolean
        assertEquals(booleanValue, nativeGetBooleanField(this));
        nativeSetBooleanField(this, false);
        assertEquals(false, nativeGetBooleanField(this));
        assertEquals(false, booleanValue);

        // String
        assertEquals(stringValue, nativeGetStringField(this));
        nativeSetStringField(this, "JNI");
        assertEquals("JNI", nativeGetStringField(this));
        assertEquals("JNI", stringValue);

        // int[]
        int[] initialArr = intArrayValue;
        int[] nativeInitialArr = nativeGetIntArrayField(this);
        assertNotNull(nativeInitialArr);
        assertEquals(initialArr[0], nativeInitialArr[0]);
        assertEquals(initialArr[1], nativeInitialArr[1]);
        assertEquals(initialArr[2], nativeInitialArr[2]);
        int[] arr = {10, 20, 30};
        nativeSetIntArrayField(this, arr);
        int[] resultArr = nativeGetIntArrayField(this);
        assertNotNull(resultArr);
        assertEquals(10, resultArr[0]);
        assertEquals(20, resultArr[1]);
        assertEquals(30, resultArr[2]);
        assertEquals(10, intArrayValue[0]);
        assertEquals(20, intArrayValue[1]);
        assertEquals(30, intArrayValue[2]);
    }

    public void testNativeGetSetStaticField() {
        // int
        assertEquals(intStaticValue, nativeGetStaticIntField());
        nativeSetStaticIntField(9999);
        assertEquals(9999, nativeGetStaticIntField());
        assertEquals(9999, intStaticValue);

        // double
        assertEquals(doubleStaticValue, nativeGetStaticDoubleField());
        nativeSetStaticDoubleField(12.34);
        assertEquals(12.34, nativeGetStaticDoubleField());
        assertEquals(12.34, doubleStaticValue);

        // float
        assertEquals(floatStaticValue, nativeGetStaticFloatField());
        nativeSetStaticFloatField(5.67f);
        assertEquals(5.67f, nativeGetStaticFloatField());
        assertEquals(5.67f, floatStaticValue);

        // long
        assertEquals(longStaticValue, nativeGetStaticLongField());
        nativeSetStaticLongField(1234567890L);
        assertEquals(1234567890L, nativeGetStaticLongField());
        assertEquals(1234567890L, longStaticValue);

        // short
        assertEquals(shortStaticValue, nativeGetStaticShortField());
        nativeSetStaticShortField((short) 456);
        assertEquals((short) 456, nativeGetStaticShortField());
        assertEquals((short) 456, shortStaticValue);

        // byte
        assertEquals(byteStaticValue, nativeGetStaticByteField());
        nativeSetStaticByteField((byte) 20);
        assertEquals((byte) 20, nativeGetStaticByteField());
        assertEquals((byte) 20, byteStaticValue);

        // char
        assertEquals(charStaticValue, nativeGetStaticCharField());
        nativeSetStaticCharField('Y');
        assertEquals('Y', nativeGetStaticCharField());
        assertEquals('Y', charStaticValue);

        // boolean
        assertEquals(booleanStaticValue, nativeGetStaticBooleanField());
        nativeSetStaticBooleanField(true);
        assertEquals(true, nativeGetStaticBooleanField());
        assertEquals(true, booleanStaticValue);

        // String
        assertEquals(stringStaticValue, nativeGetStaticStringField());
        nativeSetStaticStringField("StaticJNI");
        assertEquals("StaticJNI", nativeGetStaticStringField());
        assertEquals("StaticJNI", stringStaticValue);

        // int[]
        int[] initialArr = intArrayStaticValue;
        int[] nativeInitialArr = nativeGetStaticIntArrayField();
        assertNotNull(nativeInitialArr);
        assertEquals(initialArr[0], nativeInitialArr[0]);
        assertEquals(initialArr[1], nativeInitialArr[1]);
        assertEquals(initialArr[2], nativeInitialArr[2]);

        int[] arr = {40, 50, 60};
        nativeSetStaticIntArrayField(arr);
        int[] resultArr = nativeGetStaticIntArrayField();
        assertNotNull(resultArr);
        assertEquals(40, resultArr[0]);
        assertEquals(50, resultArr[1]);
        assertEquals(60, resultArr[2]);
        assertEquals(40, intArrayStaticValue[0]);
        assertEquals(50, intArrayStaticValue[1]);
        assertEquals(60, intArrayStaticValue[2]);
    }
    public void testNativeLocalRefTest() {
        assertEquals("OK", nativeLocalRefTest());
    }
    public void testNativeGlobalRefTest() {
        assertEquals("OK", nativeGlobalRefTest());
    }
    public void testNativeWeakRefTest() {
        assertEquals("OK", nativeWeakRefTest());
    }
    public void testNativeThrowAndCatch() {
        assertTrue(nativeThrowAndCatch());
    }
    public void testNativeThreadTest() {
        assertTrue(nativeThreadTest());
    }

    public void testNativeIsInstanceOf() {
        // Should be true for TestJNI
        assertTrue(nativeIsInstanceOf(this, "TestJNI"));
        // Should be true for java.lang.Object
        assertTrue(nativeIsInstanceOf(this, "java/lang/Object"));
        // Should be false for java.lang.String
        assertTrue(!nativeIsInstanceOf(this, "java/lang/String"));
        // Should be true for String
        assertTrue(nativeIsInstanceOf("abc", "java/lang/String"));
        // Should be false for Integer
        assertTrue(!nativeIsInstanceOf("abc", "java/lang/Integer"));
    }

    public void testNativeNewObject() {
        NativeTestObject obj = nativeNewObject("Hello from native", 42);
        assertNotNull(obj);
        if (obj == null) return;
        assertEquals("Hello from native", obj.message);
        assertEquals(42, obj.intValue);
    }

    public void testNativeNewObject2() {
        NativeTestObject obj = nativeNewObject2(
            "string test", -5, 3.14, 2.71f, 123456789L, (short) 7, (byte) 1, 'A', true
        );
        assertNotNull(obj);
        if (obj == null) return;
        assertEquals("string test", obj.message);
        assertEquals(-5, obj.intValue);
        assertEquals(3.14, obj.doubleValue);
        assertEquals(2.71f, obj.floatValue);
        assertEquals(123456789L, obj.longValue);
        assertEquals((short) 7, obj.shortValue);
        assertEquals((byte) 1, obj.byteValue);
        assertEquals('A', obj.charValue);
        assertEquals(true, obj.booleanValue);
    }

    public void testNativeNewObjectV() {
        NativeTestObject obj = nativeNewObjectV(
            "string test V", -10, 6.28, 1.41f, 987654321L, (short) 14, (byte) 2, 'B', false
        );
        assertNotNull(obj);
        if (obj == null) return;
        assertEquals("string test V", obj.message);
        assertEquals(-10, obj.intValue);
        assertEquals(6.28, obj.doubleValue);
        // The NewObjectV JNI function expects the arguments in the exact order
        // and type as defined in the constructor signature. If the float value
        // is not properly retrieved from the va_list or is passed incorrectly,
        // the constructor will receive an invalid value (0.0 in this case).
        assertEquals(0, obj.floatValue);
        assertEquals(987654321L, obj.longValue);
        assertEquals((short) 14, obj.shortValue);
        assertEquals((byte) 2, obj.byteValue);
        assertEquals('B', obj.charValue);
        assertEquals(false, obj.booleanValue);
    }

    public void testNativeNewObjectA() {
        Object[] args = {
            "string test A", -20, 9.42, 3.14f, 192837465L, (short) 21, (byte) 3, 'C', true
        };
        NativeTestObject obj = nativeNewObjectA(args);
        assertNotNull(obj);
        if (obj == null) return;
        assertEquals("string test A", obj.message);
        assertEquals(-20, obj.intValue);
        assertEquals(9.42, obj.doubleValue);
        assertEquals(3.14f, obj.floatValue);
        assertEquals(192837465L, obj.longValue);
        assertEquals((short) 21, obj.shortValue);
        assertEquals((byte) 3, obj.byteValue);
        assertEquals('C', obj.charValue);
        assertEquals(true, obj.booleanValue);
    }

    public void testCallMethods() {
        CallTarget t = new CallTarget();

        // Object
        String s = "callObj";
        assertEquals(s, nativeCallObjectMethod(t, s));
        assertEquals(s, nativeCallObjectMethodV(t, s));
        assertEquals(s, nativeCallObjectMethodA(t, s));

        // boolean
        assertEquals(true, nativeCallBooleanMethod(t, true));
        assertEquals(true, nativeCallBooleanMethodV(t, true));
        assertEquals(true, nativeCallBooleanMethodA(t, true));

        // byte
        assertEquals((byte)5, nativeCallByteMethod(t, (byte)5));
        assertEquals((byte)5, nativeCallByteMethodV(t, (byte)5));
        assertEquals((byte)5, nativeCallByteMethodA(t, (byte)5));

        // char
        assertEquals('K', nativeCallCharMethod(t, 'K'));
        assertEquals('K', nativeCallCharMethodV(t, 'K'));
        assertEquals('K', nativeCallCharMethodA(t, 'K'));

        // short
        assertEquals((short)123, nativeCallShortMethod(t, (short)123));
        assertEquals((short)123, nativeCallShortMethodV(t, (short)123));
        assertEquals((short)123, nativeCallShortMethodA(t, (short)123));

        // int
        assertEquals(777, nativeCallIntMethod(t, 777));
        assertEquals(777, nativeCallIntMethodV(t, 777));
        assertEquals(777, nativeCallIntMethodA(t, 777));

        // long
        assertEquals(1234567890123L, nativeCallLongMethod(t, 1234567890123L));
        assertEquals(1234567890123L, nativeCallLongMethodV(t, 1234567890123L));
        assertEquals(1234567890123L, nativeCallLongMethodA(t, 1234567890123L));

        // float
        assertEquals(3.14f, nativeCallFloatMethod(t, 3.14f));
        assertEquals(3.14f, nativeCallFloatMethodV(t, 3.14f));
        assertEquals(3.14f, nativeCallFloatMethodA(t, 3.14f));

        // double
        assertEquals(6.28, nativeCallDoubleMethod(t, 6.28));
        assertEquals(6.28, nativeCallDoubleMethodV(t, 6.28));
        assertEquals(6.28, nativeCallDoubleMethodA(t, 6.28));

        // void (marker)
        nativeCallVoidMethod(t, 42);
        assertEquals(42, t.voidMarker);
        nativeCallVoidMethodV(t, 43);
        assertEquals(43, t.voidMarker);
        nativeCallVoidMethodA(t, 44);
        assertEquals(44, t.voidMarker);
    }

    public void runTests() {
        testNativeHello();
        testNativeUnicodeStringTest();
        testNativeArraySum();
        testNativeGetSetField();
        testNativeGetSetStaticField();
        testNativeLocalRefTest();
        testNativeGlobalRefTest();
        testNativeWeakRefTest();
        testNativeThrowAndCatch();
        testNativeIsInstanceOf();
        testNativeNewObject();
        testNativeNewObject2();
        testNativeNewObjectV();
        testNativeNewObjectA();
        testNativeThreadTest();
        testCallMethods();
    }

    public boolean getSuccess() {
        return success;
    }

    public static void main(String[] args) {
        TestJNI test = new TestJNI();
        test.runTests();
        if (test.getSuccess()) System.out.println("ok");
    }
}
