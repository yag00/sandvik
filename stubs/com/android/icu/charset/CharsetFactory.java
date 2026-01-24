package com.android.icu.charset;

import java.nio.charset.Charset;
import java.nio.charset.UnsupportedCharsetException;

public final class CharsetFactory {

    private CharsetFactory() {}

    public static Charset create(String name) {
        if (name == null) {
            throw new IllegalArgumentException("charset name is null");
        }

        // IMPORTANT:
        // Do NOT allocate new String objects here.
        // Android expects canonical names to already be normalized.
        if (equalsIgnoreCaseAscii(name, "US-ASCII") ||
            equalsIgnoreCaseAscii(name, "ASCII")) {
            return new java.nio.charset.US_ASCII();
        }
        if (equalsIgnoreCaseAscii(name, "ISO-8859-1") ||
            equalsIgnoreCaseAscii(name, "ISO8859-1") ||
            equalsIgnoreCaseAscii(name, "latin1")) {
            return new java.nio.charset.ISO_8859_1();
        }
        if ( equalsIgnoreCaseAscii(name, "UTF-8") ||
                equalsIgnoreCaseAscii(name, "UTF8") ||
                equalsIgnoreCaseAscii(name, "UTF-16") ||
                equalsIgnoreCaseAscii(name, "UTF-16LE") ||
                equalsIgnoreCaseAscii(name, "UTF-16BE")) {

            return new java.nio.charset.UTF_8();
        }
        throw new UnsupportedCharsetException(name);
    }

    // No allocations, no char[], no String constructors
    private static boolean equalsIgnoreCaseAscii(String a, String b) {
        if (a.length() != b.length()) return false;

        for (int i = 0; i < a.length(); i++) {
            char ca = a.charAt(i);
            char cb = b.charAt(i);

            if (ca == cb) continue;

            // fold ASCII lowercase only
            if (ca >= 'a' && ca <= 'z') ca -= 32;
            if (cb >= 'a' && cb <= 'z') cb -= 32;

            if (ca != cb) return false;
        }
        return true;
    }
}
