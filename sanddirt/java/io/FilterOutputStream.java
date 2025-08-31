package java.io;
/**
 * @@sandvik modified
 * This class is the superclass of all classes that filter output
 * streams. These streams sit on top of an already existing output
 * stream (the <i>underlying</i> output stream) which it uses as its
 * basic sink of data, but possibly transforming the data along the
 * way or providing additional functionality.
 * <p>
 * The class <code>FilterOutputStream</code> itself simply overrides
 * all methods of <code>OutputStream</code> with versions that pass
 * all requests to the underlying output stream. Subclasses of
 * <code>FilterOutputStream</code> may further override some of these
 * methods as well as provide additional methods and fields.
 */
public class FilterOutputStream extends OutputStream {
    protected OutputStream out;
    /**
     * Create a FilterOutpuStream.
     * @param stream outputStream
     */
    public FilterOutputStream(OutputStream stream) {}
    /**
     * Write a byte on the stream.
     * @param value byte to write
     * @throws IOException if IO error occurs
     */
    public native void write(int value) throws IOException;
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
     * Flush the stream.
     * @throws IOException if IO error occurs
     */
    public native void flush() throws IOException;
    /**
     * Close the stream.
     * @throws IOException if IO error occurs
     */
    public native void close() throws IOException;
    protected FilterOutputStream() {}
}
