public class TestEdgeNumbers {
    public static void main(String[] args) {
        boolean success = true;

        // Integer overflow/underflow
        int max = Integer.MAX_VALUE;
        int min = Integer.MIN_VALUE;
        if (max + 1 != Integer.MIN_VALUE) { System.out.println("fail: int overflow"); success = false; }
        if (min - 1 != Integer.MAX_VALUE) { System.out.println("fail: int underflow"); success = false; }

        // Long overflow
        long lmax = Long.MAX_VALUE;
        long lmin = Long.MIN_VALUE;
        if (lmax + 1L != Long.MIN_VALUE) { System.out.println("fail: long overflow"); success = false; }
        if (lmin - 1L != Long.MAX_VALUE) { System.out.println("fail: long underflow"); success = false; }

        // Floating point extremes
        double inf = Double.POSITIVE_INFINITY;
        double ninf = Double.NEGATIVE_INFINITY;
        double nan = Double.NaN;
        if (1.0/0.0 != inf) { System.out.println("fail: positive infinity"); success = false; }
        if (-1.0/0.0 != ninf) { System.out.println("fail: negative infinity"); success = false; }
        if (!Double.isNaN(0.0/0.0)) { System.out.println("fail: NaN"); success = false; }

        // Division by zero exception
        try {
            int x = 1 / 0;
            System.out.println("fail: int div by zero");
            success = false;
        } catch (ArithmeticException e) { }

        // Negative array index
        try {
            int[] arr = new int[1];
            arr[-1] = 0;
            System.out.println("fail: negative index");
            success = false;
        } catch (ArrayIndexOutOfBoundsException e) { }

        if (success) System.out.println("ok");
    }
}

