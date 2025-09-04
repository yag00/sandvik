public class TestDefaultMethods {
    interface Greeter {
        default String greet() {
            return "hello";
        }
    }

    static class Person implements Greeter {
        // inherits default greet()
    }

    static class LoudPerson implements Greeter {
        @Override
        public String greet() {
            return "HELLO";
        }
    }

    public static void main(String[] args) {
        boolean success = true;

        Greeter p = new Person();
        Greeter lp = new LoudPerson();

        if (!p.greet().equals("hello")) {
            System.out.println("fail: Person.greet");
            success = false;
        }
        if (!lp.greet().equals("HELLO")) {
            System.out.println("fail: LoudPerson.greet");
            success = false;
        }

        if (success) {
            System.out.println("ok");
        }
    }
}
