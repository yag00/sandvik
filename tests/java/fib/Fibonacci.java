/*
 * This file is part of Sandvik project.
 * Copyright (C) 2025 Christophe Duvernois
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

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