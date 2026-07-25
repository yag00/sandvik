public class TestJNIArrays extends BaseJNITest {

    static {
        System.loadLibrary("testjni");
    }

    public native int[] nativeArraySum(int[] values);

    public void testNativeArraySum() {
        int[] arr = {1, 2, 3};
        int[] r = nativeArraySum(arr);
        assertNotNull(r);
        assertEquals(6, r[0]);
    }

    public static void main(String[] args) {
        TestJNIArrays test = new TestJNIArrays();
        test.testNativeArraySum();
        System.out.println(test.getSuccess() ? "ok" : "fail");
    }
}