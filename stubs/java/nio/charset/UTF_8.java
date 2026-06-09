package java.nio.charset;

import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.nio.charset.CharsetEncoder;

public final class UTF_8 extends Charset {

    public UTF_8() {
        super("UTF-8", new String[] {
            "UTF8",
            "US-ASCII",
            "ISO-8859-1",
            "latin1"
        });
    }

    @Override
    public boolean contains(Charset cs) {
        return true; // lie, but safe for bootstrap
    }

    @Override
    public CharsetDecoder newDecoder() {
        return new Decoder(this);
    }

    @Override
    public CharsetEncoder newEncoder() {
        return new Encoder(this);
    }

    private static final class Decoder extends CharsetDecoder {
        Decoder(Charset cs) {
            super(cs, 1.0f, 1.0f);
        }
        protected CoderResult decodeLoop(ByteBuffer in, CharBuffer out) {
            while (in.hasRemaining()) {
                if (!out.hasRemaining()) return CoderResult.OVERFLOW;
                out.put((char) (in.get() & 0x7f));
            }
            return CoderResult.UNDERFLOW;
        }
    }

    private static final class Encoder extends CharsetEncoder {
        Encoder(Charset cs) {
            super(cs, 1.0f, 1.0f);
        }
        protected CoderResult encodeLoop(CharBuffer in, ByteBuffer out) {
            while (in.hasRemaining()) {
                if (!out.hasRemaining()) return CoderResult.OVERFLOW;
                char c = in.get();
                out.put((byte) (c <= 0x7f ? c : '?'));
            }
            return CoderResult.UNDERFLOW;
        }
    }
}
