package com.android.icu.charset;

import java.nio.charset.Charset;
import java.nio.charset.spi.CharsetProvider;
import java.util.Collections;
import java.util.Iterator;

public final class CharsetProviderICU extends CharsetProvider {

    @Override
    public Charset charsetForName(String name) {
        try {
            return Charset.forName(name);
        } catch (Exception e) {
            return null;
        }
    }

    @Override
    public Iterator<Charset> charsets() {
        return Collections.emptyIterator();
    }
}
