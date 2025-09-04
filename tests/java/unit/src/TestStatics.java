public class TestStatics {
    static int counter = init();

    static int init() {
        return 99;
    }

    static int getCounter() {
        return counter;
    }

    public static void main(String[] args) {
        if (TestStatics.getCounter() == 99) {
            System.out.println("ok");
        } else {
            System.out.println("fail");
        }
    }
}
