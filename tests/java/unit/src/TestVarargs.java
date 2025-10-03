public class TestVarargs {
    static String join(String... parts) {
        StringBuilder sb = new StringBuilder();
        for (String p : parts) sb.append(p).append(",");
        return sb.toString();
    }

    public static void main(String[] args) {
        boolean success = true;

        String s = join("a", "b", "c");
        if (!s.equals("a,b,c,")) { System.out.println("fail: varargs"); success = false; }

        String t = join();
        if (!t.equals("")) { System.out.println("fail: empty varargs"); success = false; }

        if (success) System.out.println("ok");
    }
}
