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

        Class<?>[] declared = TestInnerClasses.class.getDeclaredClasses();
        if (declared.length != 3) { System.out.println("fail: getDeclaredClasses length " + declared.length); success = false; }

        if (StaticInner.class.getEnclosingClass() != TestInnerClasses.class) { System.out.println("fail: static inner getEnclosingClass"); success = false; }
        if (StaticInner.class.getDeclaringClass() != TestInnerClasses.class) { System.out.println("fail: static inner getDeclaringClass"); success = false; }
        if (NonStaticInner.class.getEnclosingClass() != TestInnerClasses.class) { System.out.println("fail: non-static inner getEnclosingClass"); success = false; }

        if (!StaticInner.class.getSimpleName().equals("StaticInner")) { System.out.println("fail: getInnerClassName " + StaticInner.class.getSimpleName()); success = false; }

        // Note: anonymous/local classes are enclosed via the EnclosingMethod annotation (not
        // EnclosingClass), which needs a java.lang.reflect.Method-wrapping helper this VM doesn't
        // have yet, so anon.getClass().getEnclosingClass() is intentionally not exercised here.

        if (success) System.out.println("ok");
    }
}
