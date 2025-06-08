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

public class DalvikTest {

    private static int testsPassed = 0;
    private static int testsFailed = 0;

    private static void check(boolean condition, String testName) {
        if (condition) {
            System.out.println("[OK] " + testName);
            testsPassed++;
        } else {
            System.out.println("[FAIL] " + testName);
            testsFailed++;
        }
    }

    public static int testIntLogicalOps(int a, int b) {
        int c = a & b;
        int d = a | b;
        int e = a ^ b;
        int f = a % b;
        int g = ~a;
        int h = a << b;
        int i = a >> b;
        int j = a >>> b;
        int k = a + b;
        return c+d+e+f+g+h+i+j+k;

    }
    public static long testLongLogicalOps(long a, long b) {
        long c = a & b;
        long d = a | b;
        long e = a ^ b;
        long f = a % b;
        long g = ~a;
        long h = a << b;
        long i = a >> b;
        long j = a >>> b;
        long k = a + b;
        return c+d+e+f+g+h+i+j+k;
    }

    public static int sub(int a, int b) {
        return a - b;
    }
    public static int add(int a, int b) {
        return a + b;
    }
    public static int mul(int a, int b) {
        return a * b;
    }
    public static int div(int a, int b) {
        return a / b;
    }
    public static int rem(int a, int b) {
        return a % b;
    }
    public static int and(int a, int b) {
        return a & b;
    }
    public static int or(int a, int b) {
        return a | b;
    }
    public static int xor(int a, int b) {
        return a ^ b;
    }
    public static int shl(int a, int b) {
        return a << b;
    }
    public static int shr(int a, int b) {
        return a >> b;
    }
    public static int ushr(int a, int b) {
        return a >>> b;
    }

    public static long sub(long a, long b) {
        return a - b;
    }
    public static long add(long a, long b) {
        return a + b;
    }
    public static long mul(long a, long b) {
        return a * b;
    }
    public static long div(long a, long b) {
        return a / b;
    }
    public static long rem(long a, long b) {
        return a % b;
    }
    public static long and(long a, long b) {
        return a & b;
    }
    public static long or(long a, long b) {
        return a | b;
    }
    public static long xor(long a, long b) {
        return a ^ b;
    }
    public static long shl(long a, long b) {
        return a << b;
    }
    public static long shr(long a, long b) {
        return a >> b;
    }
    public static long ushr(long a, long b) {
        return a >>> b;
    }

    public static float sub(float a, float b) {
        return a - b;
    }
    public static float add(float a, float b) {
        return a + b;
    }
    public static float mul(float a, float b) {
        return a * b;
    }
    public static float div(float a, float b) {
        return a / b;
    }
    public static float rem(float a, float b) {
        return a % b;
    }

    public static double sub(double a, double b) {
        return a - b;
    }
    public static double add(double a, double b) {
        return a + b;
    }
    public static double mul(double a, double b) {
        return a * b;
    }
    public static double div(double a, double b) {
        return a / b;
    }
    public static double rem(double a, double b) {
        return a % b;
    }

    // Test integer arithmetic
    public static int testIntOps(int a, int b) {
        int add = a + b;
        int sub = a - b;
        int mul = a * b;
        int div = a / b;
        int rem = a % b;
        return add + sub + mul + div + rem;
    }
    public static int testIntLit8Ops(int a) {
        int add = a + 0x7f;
        int sub = a - 0x80;
        int mul = a * 0x2;
        int div = a / 0x3;
        int rem = a % 0x4;
        int logic = (a >> 4) | 0x2;
        logic = logic ^ 0x1;
        logic = logic << 2;
        logic = logic >>> 1;
        logic = logic & 0x3f;
        return add + sub + mul + div + rem + logic;
    }

    public static int testIntLit16Ops(int a) {
        int add = a + 0x7fff;
        int sub = a - 0x8000;
        int mul = a * 0x4000;
        int div = a / 0x1000;
        int rem = a % 0x100;
        int logic = a | 0x01f0;
        logic = logic ^ 0x0AAA;
        logic = logic & 0x0fff;
        return add + sub + mul + div + rem + logic;
    }
    // Test long arithmetic
    public static long testLongOps(long a, long b) {
        long add = a + b;
        long sub = a - b;
        long mul = a * b;
        long div = a / b;
        long rem = a % b;
        return add + sub + mul + div + rem;
    }

    // Test floating-point
    public static float testFloatOps(float a, float b) {
        float add = a + b;
        float sub = a - b;
        float mul = a * b;
        float div = a / b;
        return add + sub + mul + div;
    }
    // Test double floating-point
    public static double testDoubleOps(double a, double b) {
        double add = a + b;
        double sub = a - b;
        double mul = a * b;
        double div = a / b;
        return add + sub + mul + div;
    }
    // Test not/neg ops
    public static int testNotOps(int a) {
        return ~a;
    }
    public static long testNotOps(long a) {
        return ~a;
    }
    // Test bitwise not operation for boolean (not-int)
    public static boolean testNotOps(boolean a) {
        return !a;
    }
    public static int testNegOps(int a) {
        return -a;
    }
    public static long testNegOps(long a) {
        return -a;
    }
    public static float testNegOps(float a) {
        return -a;
    }
    public static double testNegOps(double a) {
        return -a;
    }
    public static float int2long(int i) {
        return (long)i;
    }
    public static float int2float(int i) {
        return (float)i;
    }
    public static double int2Double(int i) {
        return (double) i;
    }
    public static int long2Int(long l) {
        return (int) l;
    }
    public static float long2Float(long l) {
        return (float) l;
    }
    public static double long2Double(long l) {
        return (double) l;
    }
    public static int float2Int(float f) {
        return (int) f;
    }
    public static long float2Long(float f) {
        return (long) f;
    }
    public static int double2Int(double d) {
        return (int) d;
    }
    public static long double2Long(double d) {
        return (long) d;
    }
    public static float double2Float(double d) {
        return (float) d;
    }
    public static byte int2Byte(int i) {
        return (byte) i;
    }
    public static char int2Char(int i) {
        return (char) i;
    }
    public static short int2Short(int i) {
        return (short) i;
    }


    // Test array operations
    public static double testArrayOps() {
        boolean[] boolarr = new boolean[9];
        byte[] barr = new byte[10];
        char[] carr = new char[11];
        short[] sarr = new short[12];
        int[] arr = new int[13];
        long[] larr = new long[14];
        float[] farr = new float[15];
        double[] darr = new double[16];
        String[] stringArray = new String[2];
        stringArray[0] = "Hello";
        stringArray[1] = "World";
        String s = stringArray[0] + " " + stringArray[1];
        check(s.equals("Hello World"), "string array");

        boolarr[0] = true;
        barr[0] = 1;
        carr[0] = 2;
        sarr[0] = 3;
        arr[0] = 4;
        larr[0] = 5L;
        farr[0] = 1.0f;
        darr[0] = 2.0f;
        int length = boolarr.length + arr.length + barr.length + carr.length + sarr.length + larr.length + farr.length + darr.length;
        if (boolarr[0] == true) {
            length += 1;
        }
        return length + arr[0] + barr[0] + carr[0] + sarr[0] + larr[0] + farr[0] + darr[0];
    }

    public static char[] makeCharArray() {
        return new char[] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    }
    public static short[] makeShortArray() {
        return new short[] { 0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108, 0x109 };
    }
    public static int[] makeIntArray() {
        return new int[] { 0x100001, 0x100002, 0x100003, 0x100004, 0x100005, 0x100006, 0x100007, 0x100008 };
    }
    public static long[] makeLongArray() {
        return new long[] { 0x100000001L, 0x100000002L, 0x100000003L, 0x100000004L, 0x100000005L, 0x100000006L, 0x100000007L };
    }

    // Test object operations
    static class TestObject {
        static boolean sfbool;
        static byte sfbyte;
        static char sfchar;
        static short sfshort;
        static int sfint;
        static long sflong;
        static String sfstr;

        static boolean getSfbool() { return sfbool; }
        static void setSfbool(boolean val) { sfbool = val; }
        static byte getSfbyte() { return sfbyte; }
        static void setSfbyte(byte val) { sfbyte = val; }
        static char getSfchar() { return sfchar; }
        static void setSfchar(char val) { sfchar = val; }
        static short getSfshort() { return sfshort; }
        static void setSfshort(short val) { sfshort = val; }
        static int getSfint() { return sfint; }
        static void setSfint(int val) { sfint = val; }
        static long getSflong() { return sflong; }
        static void setSflong(long val) { sflong = val; }
        static String getSfstr() { return sfstr; }
        static void setSfstr(String val) { sfstr = val; }

        boolean fbool;
        byte fbyte;
        char fchar;
        short fshort;
        int fint;
        long flong;
        String fstr;

        int getFint() { return fint; }
        void setFint(int val) { fint = val; }
        long getFlong() { return flong; }
        void setFlong(long val) { flong = val; }
        boolean getFbool() { return fbool; }
        void setFbool(boolean val) { fbool = val; }
        byte getFbyte() { return fbyte; }
        void setFbyte(byte val) { fbyte = val; }
        char getFchar() { return fchar; }
        void setFchar(char val) { fchar = val; }
        short getFshort() { return fshort; }
        void setFshort(short val) { fshort = val; }
        String getStr() { return fstr; }
        void setStr(String val) { fstr = val; }
    }

    public static TestObject testObjectRefParameter(TestObject obj) {
        obj.setFint(66);
        obj.setFlong(67L);
        obj.setFbool(true);
        obj.setFbyte((byte) 2);
        obj.setFchar((char)'B');
        obj.setFshort((short)0x0100);
        return obj;
    }

    // Test branches
    public static int testBranches(int x) {
        if (x > 0) {
            if (x > 100) {
                return 100;
            } else {
                if (x >= 10) {
                    return 10;
                } else {
                    return 1;
                }
            }
        } else if (x < 0) {
            if (x <= -10) {
                if (x < -100) {
                    return -100;
                } else {
                    return -10;
                }
            } else {
                return -1;
            }
        } else {
            return 0;
        }
    }
    public static boolean testFloatCompare(float a, float b) {
        return a < b;
    }
    public static boolean testDoubleCompare(double a, double b) {
        return a < b;
    }
    // Test switch (packed-switch)
    public static int testSwitch(int x) {
        switch (x) {
            case 1: return 10;
            case 2: return 20;
            case 3: return 30;
            default: return 0;
        }
    }
    // Test switch (sparse-switch)
    public static int testSparseSwitch(int x) {
        switch (x) {
            case 1: return 10;
            case 100: return 20;
            case 1000: return 30;
            default: return 0;
        }
    }

    // Test string operations
    public static String testStringOps() {
        String s = "Hello";
        return s + " Dalvik!";
    }

    // Test exceptions (throw, try-catch)
    public static int testExceptions(int x) {
        if (x > 0) {
            if (x >10) {
                throw new RuntimeException("Test");
            }
            return x;
        } else {
            throw new IllegalArgumentException("Invalid");
        }
    }


    public static void runTestBranching() {
        System.out.println("Running branching tests...");
        check(testBranches(5) == 1, "Positive branch");
        check(testBranches(10) == 10, "Positive branch");
        check(testBranches(1000) == 100, "Positive branch");
        check(testBranches(0) == 0, "Zero branch");
        check(testBranches(-1) == -1, "Negative branch");
        check(testBranches(-10) == -10, "Negative branch");
        check(testBranches(-1000) == -100, "Negative branch");
        check(testFloatCompare(-1.2f, 2.0f) == true, "Float compare");
        check(testFloatCompare(3.0f, 0.1f) == false, "Float compare");
        check(testDoubleCompare(-1.2, 2.0) == true, "Double compare");
        check(testDoubleCompare(3.0, 0.1) == false, "Double compare");
    }
    public static void runTestArithmetic() {
        System.out.println("Running arithmetic tests...");
        check(testIntOps(10, 3) == 54, "Integer arithmetic");
        check(testLongOps(100L, 3L) == 534L, "Long arithmetic");
        check(testFloatOps(10.5f, 2.0f) == 47.25, "Float arithmetic");
        check(testDoubleOps(-525.995d, 256.452d) == -135946.5107865896, "Double arithmetic");
        check(testIntLit8Ops(10) == 50, "Integer Literal8 arithmetic");
        check(testIntLit16Ops(10) == 166765, "Integer Literal16 arithmetic");

        check(add(10, 3) == 13, "Integer add");
        check(sub(10, 3) == 7, "Integer sub");
        check(mul(10, 3) == 30, "Integer mul");
        check(div(10, 3) == 3, "Integer div");
        check(rem(10, 3) == 1, "Integer rem");
        check(and(10, 3) == 2, "Integer and");
        check(or(10, 3) == 11, "Integer or");
        check(xor(10, 3) == 9, "Integer xor");
        check(shl(10, 2) == 40, "Integer shl");
        check(shr(10, 2) == 2, "Integer shr");
        check(ushr(10, 2) == 2, "Integer ushr");
        check(add(100L, 3L) == 103L, "Long add");
        check(sub(100L, 3L) == 97L, "Long sub");
        check(mul(100L, 3L) == 300L, "Long mul");
        check(div(100L, 3L) == 33L, "Long div");
        check(rem(100L, 3L) == 1L, "Long rem");
        check(and(100L, 3L) == 0L, "Long and");
        check(or(100L, 3L) == 103L, "Long or");
        check(xor(100L, 3L) == 103L, "Long xor");
        check(shl(100L, 2) == 400L, "Long shl");
        check(shr(100L, 2) == 25L, "Long shr");
        check(ushr(100L, 2) == 25L, "Long ushr");
        check(add(10.5f, 2.0f) == 12.5f, "Float add");
        check(sub(10.5f, 2.0f) == 8.5f, "Float sub");
        check(mul(10.5f, 2.0f) == 21.0f, "Float mul");
        check(div(10.5f, 2.0f) == 5.25f, "Float div");
        check(rem(10.5f, 2.0f) == 0.5f, "Float rem");
        check(add(-525.995, 256.452) == -269.543, "Double add");
        check(sub(-525.995, 256.452) == -782.447, "Double sub");
        check(mul(-525.995, 256.452) == -134892.46974, "Double mul");
        check(div(-525.995, 256.452) == -2.051046589615211, "Double div");
        check(rem(-525.995, 256.452) == -13.091000000000008, "Double rem");
    }
    public static void runTestLogical() {
        System.out.println("Running logical tests...");
        check(testIntLogicalOps(10, 3) == 107, "Integer logical");
        check(testLongLogicalOps(100L, 3L) == 1033L, "Long logical");
        boolean error = false;
        if (testNegOps(10) != -10) {
            error = true;
        }
        if (testNegOps(-12) != 12) {
            error = true;
        }
        if (testNegOps(100L) != -100L) {
            error = true;
        }
        if (testNegOps(-110L) != 110L) {
            error = true;
        }
        if (testNegOps(10.5f) != -10.5f) {
            error = true;
        }
        if (testNegOps(-12.5f) != 12.5f) {
            error = true;
        }
        if (testNegOps(-525.995) != 525.995) {
            error = true;
        }
        if (testNegOps(5259.95) != -5259.95) {
            error = true;
        }
        check(error == false, "Negation test");
        error = false;
        if (testNotOps(false) != true) {
            error = true;
        }
        if (testNotOps(true) != false) {
            error = true;
        }
        if (testNotOps(0xAAAA) != -43691) {
            error = true;
        }
        if (testNotOps(0xAAAAAAAAL) != -2863311531L) {
            error = true;
        }
        check(error == false, "Not test");

        check((10 & 3) == 2, "and integer test");
        check((10 | 3) == 11, "or integer test");
        check((10 ^ 3) == 9, "xor integer test");
        check((10 << 2) == 40, "shift left integer test");
        check((10 >> 2) == 2, "shift right integer test");
        check((-10 >>> 1) == 2147483643, "unsigned shift right integer test");

        check((100L & 7) == 4, "and long test");
        check((100L | 3L) == 103, "or long test");
        check((100L ^ 3L) == 103, "xor long test");
        check((100L << 2) == 400, "shift left long test");
        check((100L >> 2) == 25, "shift right long test");
        check((-100L >>> 2) == 4611686018427387879L, "unsigned shift right long test");


    }
    public static void runTestSwitch() {
        System.out.println("Running switch tests...");
        boolean error = false;
        for (int i = 0; i < 4; i++) {
            if (testSwitch(i) != i * 10) {
                error = true;
            }
        }
        if (testSparseSwitch(50) != 0) {
            error = true;
        }
        if (testSparseSwitch(1) != 10) {
            error = true;
        }
        if (testSparseSwitch(100) != 20) {
            error = true;
        }
        if (testSparseSwitch(1000) != 30) {
            error = true;
        }
        if (testSparseSwitch(2000) != 0) {
            error = true;
        }
        check(error == false, "Switch test");
    }
    public static void runTestCasts() {
        System.out.println("Running cast operation tests...");
        byte b = 123;
        char c = 123;
        int i = 123;
        short s = 123;
        long l = 123456789012345L;
        float f = 123.45f;
        double d = 123456.789;

        check(int2long(i) == 123L, "cast int-to-long");
        check(int2long(-i) == -123L, "cast int-to-long");

        check(int2float(i) == 123.0f, "cast int-to-float");
        check(int2float(-i) == -123.0f, "cast int-to-float");

        check(int2Double(i) == 123.0, "cast int-to-double");
        check(int2Double(-i) == -123.0, "cast int-to-double");

        check(long2Int(l) == -2045911175, "cast long-to-int");
        check(long2Int(-l) == 2045911175, "cast long-to-int");

        check(long2Float(l) == 123456789012345f, "cast long-to-float");
        check(long2Float(-l) == -123456789012345f, "cast long-to-float");

        check(long2Double(l) == 123456789012345.0, "cast long-to-double");
        check(long2Double(-l) == -123456789012345.0, "cast long-to-double");

        check(float2Int(f) == 123, "cast float-to-int");
        check(float2Int(-f) == -123, "cast float-to-int");

        check(float2Long(f) == 123L, "cast float-to-long");
        check(float2Long(-f) == -123L, "cast float-to-long");

        check(double2Int(d) == 123456, "cast double-to-int");
        check(double2Int(-d) == -123456, "cast double-to-int");

        check(double2Long(d) == 123456L, "cast double-to-long");
        check(double2Long(-d) == -123456L, "cast double-to-long");

        check(double2Float(d) == 123456.789f, "cast double-to-float");
        check(double2Float(-d) == -123456.789f, "cast double-to-float");

        check(int2Byte(i) == b, "cast int-to-byte");
        check(int2Byte(-i) == -b, "cast int-to-byte");

        check(int2Char(i) == c, "cast int-to-char");
        check(int2Short(i) == s, "cast int-to-short");
        check(int2Short(-i) == -s, "cast int-to-short");

        Object obj = "Hello";
        String str = (String) obj;  // Compiles to check-cast
        check(str.equals("Hello"), "cast object-to-string");
    }
    public static void runTestFieldAndStaticOps() {
        System.out.println("Running field and static operation tests...");
        TestObject obj = new TestObject();
        obj.setFint(100);
        check(obj.getFint() == 100, "iget-int");
        obj.setFlong(200L);
        check(obj.getFlong() == 200L, "iget-wide");
        obj.setFbool(true);
        check(obj.getFbool() == true, "iget-boolean");
        obj.setFbyte((byte) 1);
        check(obj.getFbyte() == (byte) 1, "iget-byte");
        obj.setFchar((char)'A');
        check(obj.getFchar() == (char)'A', "iget-char");
        obj.setFshort((short)0x0A00);
        check(obj.getFshort() == (short)0x0A00, "iget-short");
        obj.setStr("Test String");
        check(obj.getStr().equals("Test String"), "iget-object");

        TestObject.setSfint(300);
        check(TestObject.getSfint() == 300, "sget-int");
        TestObject.setSflong(400L);
        check(TestObject.getSflong() == 400L, "sget-wide");
        TestObject.setSfbool(false);
        check(TestObject.getSfbool() == false, "sget-boolean");
        TestObject.setSfbyte((byte) 2);
        check(TestObject.getSfbyte() == (byte) 2, "sget-byte");
        TestObject.setSfchar((char)'B');
        check(TestObject.getSfchar() == (char)'B', "sget-char");
        TestObject.setSfshort((short)0x0B00);
        check(TestObject.getSfshort() == (short)0x0B00, "sget-short");
        TestObject.setSfstr("Static Test String");
        check(TestObject.getSfstr().equals("Static Test String"), "sget-object");

        TestObject obj2 = testObjectRefParameter(obj);
        check(obj.getFint() == 66, "object-ref-parameter");
        check(obj.getFlong() == 67L, "object-ref-parameter");
        check(obj.getFbool() == true, "object-ref-parameter");
        check(obj.getFbyte() == (byte) 2, "object-ref-parameter");
        check(obj.getFchar() == (char)'B', "object-ref-parameter");
        check(obj.getFshort() == (short)0x0100, "object-ref-parameter");
        check(obj2.getFint() == 66, "object-ref-return");
        check(obj2.getFlong() == 67L, "object-ref-return");
        check(obj2.getFbool() == true, "object-ref-return");
        check(obj2.getFbyte() == (byte) 2, "object-ref-return");
        check(obj2.getFchar() == (char)'B', "object-ref-return");
        check(obj2.getFshort() == (short)0x0100, "object-ref-return");

        check(obj == obj2, "object-equality");
        check((obj != obj2) == false, "object-inequality");
    }
    public static void runTestStringOps() {
        System.out.println("Running string operation tests...");
        String str = testStringOps();
        check(str.equals("Hello Dalvik!"), "string ops");
        Class<?> clazz = String.class;
        check(clazz.getName().equals("java.lang.String"), "const-class");
        Object obj = str;
        boolean isString = obj instanceof String;
        check(isString, "instance-of");
    }
    public static void runTestArrayOps() {
        System.out.println("Running array operation tests...");
        double result = testArrayOps();
        check(result == 119.0, "array ops");

        char[] charArray = makeCharArray();
        for (int i = 0; i < charArray.length; i++) {
            check(charArray[i] == i + 1, "char array element " + i);
        }

        short[] shortArray = makeShortArray();
        for (int i = 0; i < shortArray.length; i++) {
            check(shortArray[i] == 0x100 + i, "short array element " + i);
        }

        int[] intArray = makeIntArray();
        for (int i = 0; i < intArray.length; i++) {
            check(intArray[i] == 0x100001 + i, "int array element " + i);
        }

        long[] longArray = makeLongArray();
        for (int i = 0; i < longArray.length; i++) {
            check(longArray[i] == 0x100000001L + i, "long array element " + i);
        }

        int[][] A = new int[3][3];
         for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                A[i][j] = i*3+j;
            }
         }
        check(A[0][0] == 0, "2D array A[0][0]");
        check(A[0][1] == 1, "2D array A[0][1]");
        check(A[0][2] == 2, "2D array A[0][2]");
        check(A[1][0] == 3, "2D array A[1][0]");
        check(A[1][1] == 4, "2D array A[1][1]");
        check(A[1][2] == 5, "2D array A[1][2]");
        check(A[2][0] == 6, "2D array A[2][0]");
        check(A[2][1] == 7, "2D array A[2][1]");
        check(A[2][2] == 8, "2D array A[2][2]");
    }
    public static void runTestInheritance() {
        interface MyInterface {
            int interfaceMethod();
        }
        class Impl implements MyInterface {
            @Override
            public int interfaceMethod() {
                return 22;
            }
        }

        class Parent {
            public Parent() {}
            public int method() {
                return 10;
            }
        }

        class Child extends Parent {
            public Child() {
                super();
            }
            @Override
            public int method() {
                int r = super.method();
                return r + 1;
            }
        }
        Parent o = new Child();
        check(o.method() == 11, "invoke");
        check(new Child().method() == 11, "invoke-super");
        MyInterface obj = new Impl();
        check(obj.interfaceMethod() == 22, "invoke-interface");
    }
    public static void runTestExceptions() {
        System.out.println("Running exception tests...");
        testExceptions(5);
        try {
            testExceptions(16);
            check(false, "Exception not thrown");
        } catch (RuntimeException e) {
            check(e.getMessage().equals("Test"), "RuntimeException Exception caught");
        }
        try {
            testExceptions(-1);
            check(false, "Exception not thrown");
        } catch (IllegalArgumentException e) {
            check(e.getMessage().equals("Invalid"), "IllegalArgumentException Exception caught");
        }
    }

    // Main method to run all tests
    public static void main(String[] args) {
        System.out.println("Running Dalvik tests...");
        runTestBranching();
        runTestArithmetic();
        runTestLogical();
        runTestCasts();
        runTestSwitch();
        runTestFieldAndStaticOps();
        runTestStringOps();
        runTestArrayOps();
        runTestInheritance();
        runTestExceptions();
        System.out.println("Tests passed: " + testsPassed);
        System.out.println("Tests failed: " + testsFailed);
    }
}