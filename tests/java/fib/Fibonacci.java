public class Fibonacci {

    /**
     * Calculates the nth Fibonacci number recursively (non-static)
     * @param n the position in the Fibonacci sequence (0-based)
     * @return the nth Fibonacci number
     */
    public int fibonacciRecursive(int n) {
        if (n <= 1) {
            return n;
        }
        return fibonacciRecursive(n - 1) + fibonacciRecursive(n - 2);
    }

    /**
     * Calculates the nth Fibonacci number iteratively (non-static)
     * @param n the position in the Fibonacci sequence (0-based)
     * @return the nth Fibonacci number
     */
    public int fibonacciIterative(int n) {
        if (n <= 1) {
            return n;
        }

        int fib = 1;
        int prevFib = 1;

        for (int i = 2; i < n; i++) {
            int temp = fib;
            fib += prevFib;
            prevFib = temp;
        }
        return fib;
    }

    public void run() {
        // Calculate Fibonacci numbers from 0 to 10
        System.out.println("Fibonacci sequence using recursive method:");
        for (int i = 0; i <= 10; i++) {
            System.out.print(fibonacciRecursive(i) + " ");
        }

        System.out.println("\n\nFibonacci sequence using iterative method:");
        for (int i = 0; i <= 10; i++) {
            System.out.print(fibonacciIterative(i) + " ");
        }

        // Calculate a specific Fibonacci number
        int specificNumber = 15;
        System.out.println("\n\nFibonacci number at position " + specificNumber + ":");
        System.out.println("Recursive: " + fibonacciRecursive(specificNumber));
        System.out.println("Iterative: " + fibonacciIterative(specificNumber));
    }

    public static void main(String[] args) {
        // Create an instance of Fibonacci
        Fibonacci fibCalculator = new Fibonacci();
        fibCalculator.run();
    }
}