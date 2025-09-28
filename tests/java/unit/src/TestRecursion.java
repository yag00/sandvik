public class TestRecursion {
    static int fact(int n) {
        if (n <= 1) return 1;
        return n * fact(n - 1);
    }

    public static void main(String[] args) {
        boolean success = true;

        int r = fact(5); // 120
        if (r != 120) { System.out.println("fail: factorial(5)"); success = false; }

        int r2 = fact(0); // 1
        if (r2 != 1) { System.out.println("fail: factorial(0)"); success = false; }

        if (success) System.out.println("ok");
    }
}
