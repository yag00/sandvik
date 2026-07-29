public class TestJNITypes extends BaseJNITest {

    static {
        System.loadLibrary("testjni");
    }

    public native boolean nativeIsInstanceOf(Object obj, String className);

    public void testNativeIsInstanceOf() {
        assertTrue(nativeIsInstanceOf(this, "TestJNITypes"));
        assertTrue(nativeIsInstanceOf(this, "java/lang/Object"));
        assertTrue(!nativeIsInstanceOf(this, "java/lang/String"));
        assertTrue(nativeIsInstanceOf("abc", "java/lang/String"));
        assertTrue(!nativeIsInstanceOf("abc", "java/lang/Integer"));
    }

    public static void main(String[] args) {
        TestJNITypes test = new TestJNITypes();
        test.testNativeIsInstanceOf();
        System.out.println(test.getSuccess() ? "ok" : "fail");
    }
}