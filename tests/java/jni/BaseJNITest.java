public abstract class BaseJNITest {
    protected boolean success = true;

    protected void assertEquals(String expected, String actual) {
        if (expected == null ? actual != null : !expected.equals(actual)) {
            System.out.println("FAIL: expected \"" + expected + "\", got \"" + actual + "\"");
            success = false;
        }
    }

    protected void assertEquals(int expected, int actual) {
        if (expected != actual) {
            System.out.println("FAIL: expected " + expected + ", got " + actual);
            success = false;
        }
    }

    protected void assertEquals(double expected, double actual) {
        if (Double.compare(expected, actual) != 0) {
            System.out.println("FAIL: expected " + expected + ", got " + actual);
            success = false;
        }
    }

    protected void assertEquals(float expected, float actual) {
        if (Float.compare(expected, actual) != 0) {
            System.out.println("FAIL: expected " + expected + ", got " + actual);
            success = false;
        }
    }

    protected void assertEquals(long expected, long actual) {
        if (expected != actual) {
            System.out.println("FAIL: expected " + expected + ", got " + actual);
            success = false;
        }
    }

    protected void assertEquals(short expected, short actual) {
        if (expected != actual) {
            System.out.println("FAIL: expected " + expected + ", got " + actual);
            success = false;
        }
    }

    protected void assertEquals(byte expected, byte actual) {
        if (expected != actual) {
            System.out.println("FAIL: expected " + expected + ", got " + actual);
            success = false;
        }
    }

    protected void assertEquals(char expected, char actual) {
        if (expected != actual) {
            System.out.println("FAIL: expected '" + expected + "', got '" + actual + "'");
            success = false;
        }
    }

    protected void assertEquals(boolean expected, boolean actual) {
        if (expected != actual) {
            System.out.println("FAIL: expected " + expected + ", got " + actual);
            success = false;
        }
    }

    protected void assertEquals(Object expected, Object actual) {
        if (expected != actual) {
            System.out.println("FAIL: expected " + expected + ", got " + actual);
            success = false;
        }
    }

    protected void assertNotNull(Object obj) {
        if (obj == null) {
            System.out.println("FAIL: expected non-null object, got null");
            success = false;
        }
    }

    protected void assertTrue(boolean condition) {
        if (!condition) {
            System.out.println("FAIL: expected true, got false");
            success = false;
        }
    }

    public boolean getSuccess() {
        return success;
    }
}