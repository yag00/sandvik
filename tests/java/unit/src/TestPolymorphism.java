public class TestPolymorphism {
    interface I { String f(); }
    static class Base implements I { public String f() { return "Base"; } }
    static class Derived extends Base { public String f() { return "Derived"; } }

    public static void main(String[] args) {
        boolean success = true;
        I i = new Derived();
        Base b = new Derived();

        if (!i.f().equals("Derived")) { System.out.println("fail: interface dispatch"); success = false; }
        if (!b.f().equals("Derived")) { System.out.println("fail: class dispatch"); success = false; }

        if (success) System.out.println("ok");
    }
}
