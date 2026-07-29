public class TestJNIHelloWorld extends BaseJNITest {

    static {
        System.loadLibrary("testjni");
    }

    public native String nativeHello(String name);

    public void testNativeHello() {
        String result = nativeHello("World");
        assertEquals("Hello World 5", result);
    }

    public static void main(String[] args) {
        TestJNIHelloWorld test = new TestJNIHelloWorld();
        test.testNativeHello();
        System.out.println(test.getSuccess() ? "ok" : "fail");
    }
}