public class TestMethodConflict {
    interface A {
        default String name() { return "A"; }
    }
    interface B {
        default String name() { return "B"; }
    }
    static class C implements A, B {
        @Override
        public String name() { return "C"; } // must resolve conflict
    }

    public static void main(String[] args) {
        C c = new C();
        if (c.name().equals("C")) {
            System.out.println("ok");
        } else {
            System.out.println("fail: conflict");
        }
    }
}

