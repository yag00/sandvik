public class TestFinal {
    static final int CONST = 42;

    static final class Immutable {
        final int x = 1;
        final int getX() { return x; }
    }

    public static void main(String[] args) {
        boolean success = true;
        if (CONST != 42) { System.out.println("fail: final field"); success = false; }

        Immutable im = new Immutable();
        if (im.getX() != 1) { System.out.println("fail: final method"); success = false; }

        if (success) System.out.println("ok");
    }
}
