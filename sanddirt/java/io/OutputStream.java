package java.io;
/**
 * @@sandvik modified
 * The {@code OutputStream} class is a root abstract class of outputStream classes.
 * <p>Nota: Native class
 */
public abstract class OutputStream implements Closeable, Flushable {
    /**
     * Write the data on the stream.
     * @param buffer array to write
     * @param offset index of the element to write
     * @param len length of the data to write
     * @throws IOException if IO error occurs
     */
    public native void write(byte[] buffer, int offset, int len) throws IOException;
    /**
     * Write the data on the stream.
     * @param buffer array to write
     * @throws IOException if IO error occurs
     */
    public native void write(byte[] buffer) throws IOException;
    /**
     * Close the stream.
     *  Do nothing in this implementation
     * @throws IOException never in this implementation
     */
    public native void close() throws IOException;
    /**
     * Flush the stream.
     *  Do nothing in this implementation
     * @throws IOException never in this implementation
     */
    public native void flush() throws IOException;
    /**
     * Write a byte on the stream.
     * @param value data to write
     * @throws IOException if IO error occurs
     */
    public abstract void write(int value) throws IOException;
}
