public class TestInvokeSuper {

    static abstract class X {
        abstract public String identify();

        public String chain() {
            return identify(); // should call A.identify() unless overridden
        }
    }

    static class A extends X {
        @Override
        public String identify() {
            return "A";
        }
    }

    static class B extends A {
        @Override
        public String chain() {
            // Explicitly call super.chain() to test invoke-super
            return super.chain() + "->B";
        }
    }

    static class C extends B {
        @Override
        public String chain() {
            return super.chain() + "->C";
        }
    }

    public static void main(String[] args) {
        boolean success = true;

        A a = new A();
        B b = new B();
        C c = new C();

        String ra = a.chain(); // "A"
        String rb = b.chain(); // "A->B"
        String rc = c.chain(); // "A->B->C"

        if (!"A".equals(ra)) {
            System.out.println("fail: A.chain() => " + ra);
            success = false;
        }
        if (!"A->B".equals(rb)) {
            System.out.println("fail: B.chain() => " + rb);
            success = false;
        }
        if (!"A->B->C".equals(rc)) {
            System.out.println("fail: C.chain() => " + rc);
            success = false;
        }

        if (success) System.out.println("ok");
    }
}