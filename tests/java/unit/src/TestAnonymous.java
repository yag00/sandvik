public class TestAnonymous {
    interface Greeting {
        String say();
    }

    public static void main(String[] args) {
        Greeting g = new Greeting() {
            @Override
            public String say() {
                return "hi";
            }
        };

        if (g.say().equals("hi")) {
            System.out.println("ok");
        } else {
            System.out.println("fail: anonymous");
        }
    }
}
