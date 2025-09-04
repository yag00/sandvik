public class TestExceptions {
    static int div(int a, int b) {
        try {
            return a / b;
        } catch (ArithmeticException e) {
            return -1;
        } finally {
            // executed always
            System.out.print("");
        }
    }

    public static void main(String[] args) {
        boolean success = true;
        if (div(10, 2) != 5) { System.out.println("fail: div ok"); success = false; }
        if (div(1, 0) != -1) { System.out.println("fail: div by zero"); success = false; }
        if (success) System.out.println("ok");
    }
}
