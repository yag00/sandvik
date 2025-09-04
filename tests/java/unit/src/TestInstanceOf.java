public class TestInstanceOf {
    interface C {}
    static class A implements C {}
    static class B extends A {}

    public static void main(String[] args) {
        boolean success = true;

        A a = new B();
        if (!(a instanceof B)) { System.out.println("fail: instanceof B"); success = false; }
        if (!(a instanceof A)) { System.out.println("fail: instanceof A"); success = false; }
        if (!(a instanceof C)) { System.out.println("fail: instanceof C"); success = false; }
        if (success) System.out.println("ok");
    }
}
