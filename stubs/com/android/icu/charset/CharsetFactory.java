package com.android.icu.charset;

import java.nio.charset.Charset;
import java.nio.charset.UnsupportedCharsetException;
import java.util.Locale;

public final class CharsetFactory {

    private CharsetFactory() {
        // no instances
    }

    /**
     * Called from java.nio.charset.Charset.lookup()
     */
    public static Charset create(String name) {
        if (name == null) {
            throw new IllegalArgumentException("charset name is null");
        }

        try {
            // Android normalizes names to upper-case US locale
            String normalized = name.toUpperCase(Locale.US);

            return Charset.forName(normalized);
        } catch (Exception e) {
            throw new UnsupportedCharsetException(name);
        }
    }
}
