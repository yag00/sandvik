public class TestJNIStrings extends BaseJNITest {

    static {
        System.loadLibrary("testjni");
    }

    public native String nativeUnicodeStringTest(String input);

    public void testNativeUnicodeStringTest() {
        String unicode = "\u0068\u0065\u006c\u006c\u006f";
        String result = nativeUnicodeStringTest(unicode);
        assertEquals(unicode, result);
    }

    public static void main(String[] args) {
        TestJNIStrings test = new TestJNIStrings();
        test.testNativeUnicodeStringTest();
        System.out.println(test.getSuccess() ? "ok" : "fail");
    }
}