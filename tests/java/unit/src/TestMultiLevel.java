public class TestMultiLevel {
    interface Identifiable {
        String id();
    }

    static class Entity implements Identifiable {
        @Override
        public String id() {
            return "E";
        }
    }

    static class User extends Entity {
        @Override
        public String id() {
            return "U";
        }
    }

    static class Admin extends User {
        @Override
        public String id() {
            return "A";
        }
    }

    public static void main(String[] args) {
        boolean success = true;

        Identifiable e = new Entity();
        Identifiable u = new User();
        Identifiable a = new Admin();

        if (!e.id().equals("E")) {
            System.out.println("fail: Entity.id");
            success = false;
        }
        if (!u.id().equals("U")) {
            System.out.println("fail: User.id");
            success = false;
        }
        if (!a.id().equals("A")) {
            System.out.println("fail: Admin.id");
            success = false;
        }

        if (success) {
            System.out.println("ok");
        }
    }
}
