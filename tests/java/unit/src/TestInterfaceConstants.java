public class TestInterfaceConstants {
    interface Config {
        int MAGIC = 1234; // implicitly public static final
    }

    public static void main(String[] args) {
        if (Config.MAGIC == 1234) {
            System.out.println("ok");
        } else {
            System.out.println("fail: constant");
        }
    }
}

