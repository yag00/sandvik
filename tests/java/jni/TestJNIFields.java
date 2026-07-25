public class TestJNIFields extends BaseJNITest {

    static {
        System.loadLibrary("testjni");
    }

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

    // Native Instance Fields
    public native int nativeGetIntField(TestJNIFields obj);
    public native void nativeSetIntField(TestJNIFields obj, int v);
    public native double nativeGetDoubleField(TestJNIFields obj);
    public native void nativeSetDoubleField(TestJNIFields obj, double v);
    public native float nativeGetFloatField(TestJNIFields obj);
    public native void nativeSetFloatField(TestJNIFields obj, float v);
    public native long nativeGetLongField(TestJNIFields obj);
    public native void nativeSetLongField(TestJNIFields obj, long v);
    public native short nativeGetShortField(TestJNIFields obj);
    public native void nativeSetShortField(TestJNIFields obj, short v);
    public native byte nativeGetByteField(TestJNIFields obj);
    public native void nativeSetByteField(TestJNIFields obj, byte v);
    public native char nativeGetCharField(TestJNIFields obj);
    public native void nativeSetCharField(TestJNIFields obj, char v);
    public native boolean nativeGetBooleanField(TestJNIFields obj);
    public native void nativeSetBooleanField(TestJNIFields obj, boolean v);
    public native String nativeGetStringField(TestJNIFields obj);
    public native void nativeSetStringField(TestJNIFields obj, String v);
    public native int[] nativeGetIntArrayField(TestJNIFields obj);
    public native void nativeSetIntArrayField(TestJNIFields obj, int[] v);

    // Native Static Fields
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

    public void testNativeGetSetField() {
        assertEquals(intValue, nativeGetIntField(this));
        nativeSetIntField(this, 1234);
        assertEquals(1234, nativeGetIntField(this));
        assertEquals(1234, intValue);

        assertEquals(doubleValue, nativeGetDoubleField(this));
        nativeSetDoubleField(this, 9.99);
        assertEquals(9.99, nativeGetDoubleField(this));
        assertEquals(9.99, doubleValue);

        assertEquals(floatValue, nativeGetFloatField(this));
        nativeSetFloatField(this, 8.88f);
        assertEquals(8.88f, nativeGetFloatField(this));
        assertEquals(8.88f, floatValue);

        assertEquals(longValue, nativeGetLongField(this));
        nativeSetLongField(this, 987654321L);
        assertEquals(987654321L, nativeGetLongField(this));
        assertEquals(987654321L, longValue);

        assertEquals(shortValue, nativeGetShortField(this));
        nativeSetShortField(this, (short) 123);
        assertEquals((short) 123, nativeGetShortField(this));
        assertEquals((short) 123, shortValue);

        assertEquals(byteValue, nativeGetByteField(this));
        nativeSetByteField(this, (byte) 7);
        assertEquals((byte) 7, nativeGetByteField(this));
        assertEquals((byte) 7, byteValue);

        assertEquals(charValue, nativeGetCharField(this));
        nativeSetCharField(this, 'Z');
        assertEquals('Z', nativeGetCharField(this));
        assertEquals('Z', charValue);

        assertEquals(booleanValue, nativeGetBooleanField(this));
        nativeSetBooleanField(this, false);
        assertEquals(false, nativeGetBooleanField(this));
        assertEquals(false, booleanValue);

        assertEquals(stringValue, nativeGetStringField(this));
        nativeSetStringField(this, "JNI");
        assertEquals("JNI", nativeGetStringField(this));
        assertEquals("JNI", stringValue);

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
        assertEquals(intStaticValue, nativeGetStaticIntField());
        nativeSetStaticIntField(9999);
        assertEquals(9999, nativeGetStaticIntField());
        assertEquals(9999, intStaticValue);

        assertEquals(doubleStaticValue, nativeGetStaticDoubleField());
        nativeSetStaticDoubleField(12.34);
        assertEquals(12.34, nativeGetStaticDoubleField());
        assertEquals(12.34, doubleStaticValue);

        assertEquals(floatStaticValue, nativeGetStaticFloatField());
        nativeSetStaticFloatField(5.67f);
        assertEquals(5.67f, nativeGetStaticFloatField());
        assertEquals(5.67f, floatStaticValue);

        assertEquals(longStaticValue, nativeGetStaticLongField());
        nativeSetStaticLongField(1234567890L);
        assertEquals(1234567890L, nativeGetStaticLongField());
        assertEquals(1234567890L, longStaticValue);

        assertEquals(shortStaticValue, nativeGetStaticShortField());
        nativeSetStaticShortField((short) 456);
        assertEquals((short) 456, nativeGetStaticShortField());
        assertEquals((short) 456, shortStaticValue);

        assertEquals(byteStaticValue, nativeGetStaticByteField());
        nativeSetStaticByteField((byte) 20);
        assertEquals((byte) 20, nativeGetStaticByteField());
        assertEquals((byte) 20, byteStaticValue);

        assertEquals(charStaticValue, nativeGetStaticCharField());
        nativeSetStaticCharField('Y');
        assertEquals('Y', nativeGetStaticCharField());
        assertEquals('Y', charStaticValue);

        assertEquals(booleanStaticValue, nativeGetStaticBooleanField());
        nativeSetStaticBooleanField(true);
        assertEquals(true, nativeGetStaticBooleanField());
        assertEquals(true, booleanStaticValue);

        assertEquals(stringStaticValue, nativeGetStaticStringField());
        nativeSetStaticStringField("StaticJNI");
        assertEquals("StaticJNI", nativeGetStaticStringField());
        assertEquals("StaticJNI", stringStaticValue);

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

    public static void main(String[] args) {
        TestJNIFields test = new TestJNIFields();
        test.testNativeGetSetField();
        test.testNativeGetSetStaticField();
        System.out.println(test.getSuccess() ? "ok" : "fail");
    }
}