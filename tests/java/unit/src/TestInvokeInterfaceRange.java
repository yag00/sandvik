public class TestInvokeInterfaceRange {
    
    static interface IA {
        public String invokeIdentify(String s, String t, String r, String i, String n, String g);
    }

     static class A implements IA {
        public String invokeIdentify(String s, String t, String r, String i, String n, String g) {
            return s + t + r + i + n + g;
        }
    }

    public static void main(String[] args) {
        boolean success = true;

        IA a = new A();
    
        String ra = a.invokeIdentify("A", "B", "C", "D", "E", "F"); // "ABCDEF"
        if (!"ABCDEF".equals(ra)) {
            System.out.println("fail: invokeIdentify(A,B,C,D,E,F) => " + ra);
            success = false;
        }

        ra = a.invokeIdentify(null, "B", "C", null, "E", null); // "ABCDEF"
        if (!"nullBCnullEnull".equals(ra)) {
            System.out.println("fail: invokeIdentify(null,B,C,null,E,null) => " + ra);
            success = false;
        }

        if (success) System.out.println("ok");
    }
}

