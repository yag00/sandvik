public class TestArithmetic {
    public static void main(String[] args) {
        int a = 5, b = 3;
        float x = 2.5f, y = 4.0f;
        boolean success = true;

        if (a + b != 8) { System.out.println("fail: int add"); success = false; }
        if (a * b != 15) { System.out.println("fail: int mul"); success = false; }
        if (a / b != 1) { System.out.println("fail: int div"); success = false; }
        if (x * y != 10.0f) { System.out.println("fail: float mul"); success = false; }
        if ((byte)130 != -126) { System.out.println("fail: cast byte"); success = false; }

        if (success) System.out.println("ok");
    }
}

