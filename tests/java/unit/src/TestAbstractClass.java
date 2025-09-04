public class TestAbstractClass {
    interface Animal {
        String sound();
    }

    static abstract class Mammal implements Animal {
        public String type() {
            return "mammal";
        }
    }

    static class Dog extends Mammal {
        @Override
        public String sound() {
            return "woof";
        }
    }

    public static void main(String[] args) {
        boolean success = true;

        Animal a = new Dog();
        Mammal m = new Dog();

        if (!a.sound().equals("woof")) {
            System.out.println("fail: Dog.sound");
            success = false;
        }
        if (!m.type().equals("mammal")) {
            System.out.println("fail: Mammal.type");
            success = false;
        }
        if (!(a instanceof Dog)) {
            System.out.println("fail: a instanceof Dog");
            success = false;
        }

        if (success) {
            System.out.println("ok");
        }
    }
}
