public class TestMonitorException {

    static class ExceptionTest {
        static int reached = 0;

        static synchronized void explode() {
            reached++;
            throw new RuntimeException("boom");
        }
    }

    public static void main(String[] args) {
        boolean success = true;

        try {
            ExceptionTest.explode();
            System.out.println("fail: exception not thrown");
            success = false;
        } catch (RuntimeException e) {
            // expected
        }

        // Try again — must not deadlock
        try {
            ExceptionTest.explode();
        } catch (RuntimeException e) {
            // expected
        }

        if (ExceptionTest.reached != 2) {
            System.out.println("fail: reached=" + ExceptionTest.reached);
            success = false;
        }

        if (success) System.out.println("ok");
    }
}
