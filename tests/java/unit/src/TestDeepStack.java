public class TestDeepStack {
    public static void main(String[] args) {
        boolean success = true;

        int x = (((((((1 + 2) * 3) - 4) * 5) + 6) * 7) - 8) * 9;
        if (x != 1881) { System.out.println("fail: deep stack " + x); success = false; }

        if (success) System.out.println("ok");
    }
}
