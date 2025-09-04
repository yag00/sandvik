 public class TestInterfaceStatic {
    interface MathUtil {
        static int square(int x) { return x * x; }
    }

    public static void main(String[] args) {
        if (MathUtil.square(5) == 25) {
            System.out.println("ok");
        } else {
            System.out.println("fail: static");
        }
    }
}
