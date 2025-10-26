package java.io;
/**
 * @@sandvik modified
 * A <code>PrintStream</code> adds functionality to another output stream,
 * namely the ability to print representations of various data values
 * conveniently.  Two other features are provided as well.  Unlike other output
 * streams, a <code>PrintStream</code> never throws an
 * <code>IOException</code>; instead, exceptional situations merely set an
 * internal flag that can be tested via the <code>checkError</code> method.
 * Optionally, a <code>PrintStream</code> can be created so as to flush
 * automatically; this means that the <code>flush</code> method is
 * automatically invoked after a byte array is written, one of the
 * <code>println</code> methods is invoked, or a newline character or byte
 * (<code>'\n'</code>) is written.
 *
 * <p> All characters printed by a <code>PrintStream</code> are converted into
 * bytes using the platform's default character encoding.  The <code>{@link
 * PrintWriter}</code> class should be used in situations that require writing
 * characters rather than bytes.
 */
public class PrintStream extends FilterOutputStream {
    private int file;
    /**
     * Create a PrintStream with the given name.
     * @param fileName name of the file
     * @throws FileNotFoundException if file can not be created
     */
    public PrintStream(java.lang.String fileName) throws FileNotFoundException {}
    /**
     * Print a boolean on the stream.
     * @param value boolean to print
     */
    public native void print(boolean value);
    /**
     * Print a char on the stream.
     * @param value char to print
     */
    public native void print(char value);
    /**
     * Print an array of char on the stream.
     * @param value array of char to print
     */
    public native void print(char[] value);
    /**
     * Print a float on the stream.
     * @param value float to print
     */
    public native void print(float value);
    /**
     * Print a double on the stream.
     * @param value double to print
     */
    public native void print(double value);
    /**
     * Print a long on the stream.
     * @param value long to print
     */
    public native void print(long value);
    /**
     * Print an integer on the stream.
     * @param value integer to print
     */
    public native void print(int value);
    /**
     * Print a string on the stream.
     * @param value string to print
     */
    public native void print(java.lang.String value);
    /**
     * Print an object on the stream.
     * @param value object to print
     */
    public native void print(java.lang.Object value);
    /**
     * Print a newline.
     */
    public native void println();
    /**
     * Print a string on the stream followed by newline.
     * @param value string to print
     */
    public native void println(java.lang.String value);
    /**
     * Print a object on the stream followed by newline.
     * @param value object to print
     */
    public native void println(java.lang.Object value);
    /**
     * Print a boolean on the stream followed by newline.
     * @param value boolean to print
     */
    public native void println(boolean value);
    /**
     * Print a char on the stream followed by newline.
     * @param value char to print
     */
    public native void println(char value);
    /**
     * Print an array of char on the stream followed by newline.
     * @param value array of char to print
     */
    public native void println(char[] value);
    /**
     * Print a float on the stream followed by newline.
     * @param value float to print
     */
    public native void println(float value);
    /**
     * Print a double on the stream followed by newline.
     * @param value double to print
     */
    public native void println(double value);
    /**
     * Print a long on the stream followed by newline.
     * @param value long to print
     */
    public native void println(long value);
    /**
     * Print an integer on the stream followed by newline.
     * @param value integer to print
     */
    public native void println(int value);
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

    public native PrintStream format(String format, Object... args);
}
