public class Native {

    // Load the native library
    static {
        try {
            System.loadLibrary("native");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Failed to load native library 'libnative.so'");
            System.err.println("Please ensure the library is in your java.library.path");
            System.exit(1);
        }
    }

    // Declare the native method
    public native int test(String str, int a, int b);
    public native double doubleadd(double a, double b);
    public native long longadd(long a, long b);

    public static void main(String[] args) {
        Native example = new Native();

        // Example usage
        String inputString = "HelloNative";
        int x = 10;
        int y = 20;

        // Call the native method
        int result = example.test(inputString, x, y);

        System.out.println("Native test function returned: " + result);
        System.out.println("Native add function returned: " + example.doubleadd(5.1, 3.2));
        System.out.println("Native add function returned: " + example.longadd(10000000000L, 20000000000L));
    }
}