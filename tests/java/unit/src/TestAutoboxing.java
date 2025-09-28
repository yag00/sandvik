public class TestAutoboxing {
    public static void main(String[] args) {
        boolean success = true;

        Integer a = 10;  // autobox
        int b = a;       // unbox
        if (b != 10) { System.out.println("fail: autobox/unbox"); success = false; }

        Integer c1 = 127;
        Integer c2 = 127;
        if (c1 != c2) { System.out.println("fail: Integer cache 127"); success = false; }

        Integer d1 = 128;
        Integer d2 = 128;
        if (d1 == d2) { System.out.println("fail: Integer cache 128"); success = false; }

        if (success) System.out.println("ok");
    }
}
