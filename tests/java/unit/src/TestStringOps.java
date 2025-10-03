public class TestStringOps {
    public static void main(String[] args) {
        boolean success = true;

        String s1 = "Hello";
        String s2 = "World";
        String s3 = s1 + " " + s2;
        if (!s3.equals("Hello World")) { System.out.println("fail: concat"); success = false; }

        if (!s3.substring(6).equals("World")) { System.out.println("fail: substring"); success = false; }

        String s4 = "Hello World";
        if (s3.hashCode() != s4.hashCode()) { System.out.println("fail: hashCode"); success = false; }
        if (s3.intern() != s4.intern()) { System.out.println("fail: intern"); success = false; }

        if (success) System.out.println("ok");
    }
}
