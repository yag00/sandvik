public class TestJNIObjects extends BaseJNITest {

    static {
        System.loadLibrary("testjni");
    }

    public native NativeTestObject nativeNewObject(String msg, int i);
    public native NativeTestObject nativeNewObject2(String msg, int i, double d, float f, long l, short s, byte b, char c, boolean bool);
    public native NativeTestObject nativeNewObjectV(String msg, int i, double d, float f, long l, short s, byte b, char c, boolean bool);
    public native NativeTestObject nativeNewObjectA(Object[] args);

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

    public static void main(String[] args) {
        TestJNIObjects test = new TestJNIObjects();
        test.testNativeNewObject();
        test.testNativeNewObject2();
        test.testNativeNewObjectV();
        test.testNativeNewObjectA();
        System.out.println(test.getSuccess() ? "ok" : "fail");
    }
}