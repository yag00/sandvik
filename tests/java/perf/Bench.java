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

/** Deterministic VM micro-workloads. Usage: Bench [workload|all]; prints "name checksum". */
public class Bench {
    interface Shape { int area(); }
    static class Sq implements Shape { int s; Sq(int s) { this.s = s; } public int area() { return s * s; } }
    static class Rect implements Shape { int w, h; Rect(int w, int h) { this.w = w; this.h = h; } public int area() { return w * h; } }
    static class Tri implements Shape { int b, h; Tri(int b, int h) { this.b = b; this.h = h; } public int area() { return b * h / 2; } }

    static class Counter { int count; long total; void add(int v) { count++; total += v; } }

    static int fib(int n) { return n <= 1 ? n : fib(n - 1) + fib(n - 2); }

    static long fibRecursive() { return fib(24); }

    static long loopArith() {
        long acc = 0;
        for (int i = 0; i < 1000000; i++) {
            acc += (i * 31) ^ (i >> 3);
            acc %= 1000003;
        }
        return acc;
    }

    static long arrays() {
        int[] a = new int[2000];
        long sum = 0;
        for (int r = 0; r < 200; r++) {
            for (int i = 0; i < a.length; i++) a[i] = i * r;
            for (int i = 0; i < a.length; i++) sum += a[i];
        }
        return sum;
    }

    static long fields() {
        Counter c = new Counter();
        for (int i = 0; i < 300000; i++) c.add(i & 15);
        return c.total + c.count;
    }

    static long virtualCalls() {
        Shape[] shapes = { new Sq(3), new Rect(2, 5), new Tri(4, 6) };
        long sum = 0;
        for (int i = 0; i < 300000; i++) sum += shapes[i % 3].area();
        return sum;
    }

    static long allocation() {
        long sum = 0;
        for (int i = 0; i < 100000; i++) {
            Counter c = new Counter();
            c.add(i);
            sum += c.total;
        }
        return sum;
    }

    static long strings() {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < 5000; i++) sb.append(i % 10);
        String s = sb.toString();
        long sum = 0;
        for (int i = 0; i < s.length(); i++) sum += s.charAt(i);
        return sum + s.length();
    }

    static long collections() {
        java.util.ArrayList<Integer> list = new java.util.ArrayList<Integer>();
        for (int i = 0; i < 5000; i++) list.add(i);
        long sum = 0;
        for (int i = 0; i < list.size(); i++) sum += list.get(i);
        java.util.HashMap<Integer, Integer> map = new java.util.HashMap<Integer, Integer>();
        for (int i = 0; i < 2000; i++) map.put(i, i * 2);
        for (int i = 0; i < 2000; i++) sum += map.get(i);
        return sum;
    }

    static long run(String name) {
        switch (name) {
            case "fib": return fibRecursive();
            case "loop": return loopArith();
            case "arrays": return arrays();
            case "fields": return fields();
            case "virtual": return virtualCalls();
            case "alloc": return allocation();
            case "strings": return strings();
            case "collections": return collections();
            default: throw new IllegalArgumentException(name);
        }
    }

    static final String[] ALL = { "fib", "loop", "arrays", "fields", "virtual", "alloc", "strings", "collections" };

    public static void main(String[] args) {
        String sel = args.length > 0 ? args[0] : "all";
        for (String n : ALL) {
            if (sel.equals("all") || sel.equals(n)) System.out.println(n + " " + run(n));
        }
    }
}
