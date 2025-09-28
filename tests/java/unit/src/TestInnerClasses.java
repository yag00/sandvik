public class TestInnerClasses {
    static class StaticInner {
        int f() { return 1; }
    }

    class NonStaticInner {
        int f() { return 2; }
    }

    interface I { int f(); }

    public static void main(String[] args) {
        boolean success = true;

        StaticInner si = new StaticInner();
        if (si.f() != 1) { System.out.println("fail: static inner"); success = false; }

        TestInnerClasses outer = new TestInnerClasses();
        NonStaticInner ni = outer.new NonStaticInner();
        if (ni.f() != 2) { System.out.println("fail: non-static inner"); success = false; }

        I anon = new I() { public int f() { return 3; } };
        if (anon.f() != 3) { System.out.println("fail: anonymous inner"); success = false; }

        if (success) System.out.println("ok");
    }
}
