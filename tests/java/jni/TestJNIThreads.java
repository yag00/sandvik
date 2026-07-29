public class TestJNIThreads extends BaseJNITest {

    static {
        System.loadLibrary("testjni");
    }

    public native boolean nativeThreadTest();

    public void testNativeThreadTest() {
        assertTrue(nativeThreadTest());
    }

    public static void main(String[] args) {
        TestJNIThreads test = new TestJNIThreads();
        test.testNativeThreadTest();
        System.out.println(test.getSuccess() ? "ok" : "fail");
    }
}