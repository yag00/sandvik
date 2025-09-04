public class TestDiamond {
    interface X {
        String foo();
    }

    interface Y extends X {
    }

    interface Z extends X {
    }

    static class Impl implements Y, Z {
        @Override
        public String foo() {
            return "bar";
        }
    }

    public static void main(String[] args) {
        boolean success = true;

        X x = new Impl();
        Y y = new Impl();
        Z z = new Impl();

        if (!x.foo().equals("bar")) {
            System.out.println("fail: X.foo");
            success = false;
        }
        if (!y.foo().equals("bar")) {
            System.out.println("fail: Y.foo");
            success = false;
        }
        if (!z.foo().equals("bar")) {
            System.out.println("fail: Z.foo");
            success = false;
        }

        if (success) {
            System.out.println("ok");
        }
    }
}
