public class TestJNIExceptions extends BaseJNITest {

    static {
        System.loadLibrary("testjni");
    }

    public native boolean nativeThrowAndCatch();

    public void testNativeThrowAndCatch() {
        assertTrue(nativeThrowAndCatch());
    }

    public static void main(String[] args) {
        TestJNIExceptions test = new TestJNIExceptions();
        test.testNativeThrowAndCatch();
        System.out.println(test.getSuccess() ? "ok" : "fail");
    }
}