package java.io;
/**
 * @@sandvik modified
 * This abstract class is the superclass of all classes representing
 * an input stream of bytes.
 *
 * <p> Applications that need to define a subclass of <code>InputStream</code>
 * must always provide a method that returns the next byte of input.
 */
public abstract class InputStream implements Closeable {
    /**
     * Indicates the number bytes available to read.
     * @return always 0 in this implementation (don't know the number of available bytes)
     * @throws IOException never in this implementation
     */
    public native int available() throws IOException;
    /**
     * Read the stream and fill the buffer with read data.
     * @param buffer array to fill
     * @param offset index of the element to fill
     * @param len length of the data to read
     * @return number of bytes read or -1 if EOF is reached
     * @throws IOException if IO error occurs
     */
    public native int read(byte[] buffer, int offset, int len) throws IOException;
    /**
     * Read the stream and fill the buffer with read data.
     * @param buffer array to fill
     * @return number of bytes read or -1 if EOF is reached
     * @throws IOException if IO error occurs
     */
    public native int read(byte[] buffer) throws IOException;
    /**
     * Not implemented (Do nothing).
     * @param readlimit not implemented
     */
    public native void mark(int readlimit);
    /**
     * Indicates if {@code mark} is supported.
     * @return false in this implementation
     */
    public native boolean markSupported();
    /**
     * Not implemented.
     * @throws IOException always in this implementation
     */
    public native void reset() throws IOException;
    /**
     * Skip a number of bytes of data on the stream.
     * @param n number of bytes to skip
     * @return number of bytes skipped
     * @throws IOException if IO error occurs
     */
    public native long skip(long n) throws IOException;
    /**
     * Close the stream.
     *  Do nothing in this implementation
     * @throws IOException never in this implementation
     */
    public native void close() throws IOException;
    /**
     * Read a byte on the stream.
     * @return byte read or -1 if EOF is reached
     * @throws IOException if IO error occurs
     */
    public abstract int read() throws IOException;
}
