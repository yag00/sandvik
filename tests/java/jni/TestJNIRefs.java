public class TestJNIRefs extends BaseJNITest {

    static {
        System.loadLibrary("testjni");
    }

    public native String nativeLocalRefTest();
    public native String nativeGlobalRefTest();
    public native String nativeWeakRefTest();
    public native boolean nativeThrowAndCatch();
    public native boolean nativeThreadTest();

    public void testNativeLocalRefTest() {
        assertEquals("OK", nativeLocalRefTest());
    }

    public void testNativeGlobalRefTest() {
        assertEquals("OK", nativeGlobalRefTest());
    }

    public void testNativeWeakRefTest() {
        assertEquals("OK", nativeWeakRefTest());
    }

    public static void main(String[] args) {
        TestJNIRefs test = new TestJNIRefs();
        test.testNativeLocalRefTest();
        test.testNativeGlobalRefTest();
        test.testNativeWeakRefTest();
        System.out.println(test.getSuccess() ? "ok" : "fail");
    }
}