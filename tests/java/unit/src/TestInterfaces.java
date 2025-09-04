public class TestInterfaces {
    interface A {
        int getValue();
    }

    interface B {
        String getName();
    }

    interface C extends A, B {
        boolean isValid();
    }

    static class Base implements A {
        @Override
        public int getValue() {
            return 42;
        }
    }

    static class Derived extends Base implements C {
        @Override
        public String getName() {
            return "Derived";
        }

        @Override
        public boolean isValid() {
            return true;
        }
    }

    public static void main(String[] args) {
        boolean success = true;

        A a = new Derived();
        B b = (B) a;     // test cast to interface
        C c = (C) a;     // test cast to multiple interface inheritance

        // Check A
        if (a.getValue() != 42) {
            System.out.println("fail: A.getValue");
            success = false;
        }

        // Check B
        if (!b.getName().equals("Derived")) {
            System.out.println("fail: B.getName");
            success = false;
        }

        // Check C
        if (!c.isValid()) {
            System.out.println("fail: C.isValid");
            success = false;
        }

        // Polymorphism + instanceof checks
        if (!(a instanceof Derived)) {
            System.out.println("fail: a instanceof Derived");
            success = false;
        }
        if (!(b instanceof C)) {
            System.out.println("fail: b instanceof C");
            success = false;
        }
        if (!(c instanceof A)) {
            System.out.println("fail: c instanceof A");
            success = false;
        }

        if (success) {
            System.out.println("ok");
        }
    }
}
