public class TestStrings {
    public static void main(String[] args) {
        boolean success = true;
        String s1 = "hello";
        String s2 = "he" + "llo";
        String s3 = new String("hello");

        if (s1 != s2) { System.out.println("fail: constant pool"); success = false; }
        if (!s1.equals(s3)) { System.out.println("fail: equals"); success = false; }

        StringBuilder sb = new StringBuilder();
        sb.append("a").append(1).append("b");
        if (!sb.toString().equals("a1b")) { System.out.println("fail: StringBuilder"); success = false; }

        if (success) System.out.println("ok");
    }
}
