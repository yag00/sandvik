public class TestExceptionsNoRehandle {

    static final Object lock = new Object();

    static void inner() {
        synchronized (lock) {
            // Force a RuntimeException inside synchronized block
            Object x = null;
            x.toString(); // NullPointerException
        }
    }

    static void handler() {
        try {
            inner();
            System.out.println("fail: exception not thrown");
        } catch (Throwable t) {
            // Catch-all handler
            //System.out.println("handler caught: " + t.getClass().getSimpleName());

            // CRITICAL PART:
            // Throw again from inside the handler
            throw new RuntimeException("rethrown from handler");
        }
    }

    public static void main(String[] args) {
        boolean success = false;

        try {
            handler();
            System.out.println("fail: rethrow did not escape");
        } catch (RuntimeException e) {
            // Must reach here EXACTLY ONCE
            if ("rethrown from handler".equals(e.getMessage())) {
                success = true;
            } else {
                System.out.println("fail: wrong exception message");
            }
        } catch (Throwable t) {
            System.out.println("fail: wrong exception type");
        }

        if (success) {
            System.out.println("ok");
        }
    }
}
