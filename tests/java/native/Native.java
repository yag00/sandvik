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

public class Native {

    // Load the native library
    static {
        try {
            System.loadLibrary("native");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Failed to load native library 'libnative.so'");
            System.err.println("Please ensure the library is in your java.library.path");
            System.exit(1);
        }
    }

    // Declare the native method
    public native int test(String str, int a, int b);
    public native long longadd(long a, long b);
    public native float floatadd(float a, float b);
    public native double doubleadd(double a, double b);

    public static void main(String[] args) {
        Native example = new Native();

        // Example usage
        String inputString = "HelloNative";
        int x = 10;
        int y = 20;
        float f = 5.5f;
        float f2 = -1.1f;
        // Call the native method
        int result = example.test(inputString, x, y);

        System.out.println("Native test function returned: " + result);
        System.out.println("Native add function returned: " + example.doubleadd(5.1, 3.2));
        System.out.println("Native add function returned: " + example.floatadd(f, f2));
        System.out.println("Native add function returned: " + example.longadd(10000000000L, 20000000000L));
    }
}