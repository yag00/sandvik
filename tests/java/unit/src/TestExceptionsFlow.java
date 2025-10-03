public class TestExceptionsFlow {
    static int risky(int x) throws Exception {
        if (x == 0) throw new IllegalArgumentException("bad");
        if (x < 0) throw new Exception("general");
        return x;
    }

    public static void main(String[] args) {
        boolean success = true;

        try {
            risky(1);
        } catch (Exception e) {
            System.out.println("fail: no exception expected");
            success = false;
        }

        try {
            risky(0);
            System.out.println("fail: expected exception");
            success = false;
        } catch (IllegalArgumentException e) {
            // expected
        } catch (Exception e) {
            System.out.println("fail: unexpected exception");
            success = false;
        }

        try {
            risky(-1);
            System.out.println("fail: expected exception");
            success = false;
        } catch (IllegalArgumentException e) {
            System.out.println("fail: wrong handler");
            success = false;
        } catch (Exception e) {
            // expected
        } finally {
            // always executes
            int test = 42;
            if (test != 42) { System.out.println("fail: finally"); success = false; }
        }

        if (success) System.out.println("ok");
    }
}
