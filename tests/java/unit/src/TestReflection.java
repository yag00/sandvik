class ReflectTarget {
    public int field = 42;
    public String hello(String name) { return "Hello " + name; }
}

public class TestReflection {
    public static void main(String[] args) {
        boolean success = true;

        try {
            Class<?> cls = Class.forName("ReflectTarget");
            Object obj = cls.getDeclaredConstructor().newInstance();

            // Field access
            int val = (int) cls.getField("field").get(obj);
            if (val != 42) { System.out.println("fail: reflection field"); success = false; }

            // Method call
            String out = (String) cls.getMethod("hello", String.class).invoke(obj, "VM");
            if (!out.equals("Hello VM")) { System.out.println("fail: reflection invoke"); success = false; }
        } catch (Exception e) {
            e.printStackTrace();
            success = false;
        }

        if (success) System.out.println("ok");
    }
}