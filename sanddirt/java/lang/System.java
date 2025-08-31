package java.lang;

import java.io.*;

/**
 * @@sandvik modified
 * The System class contains several useful static fields and methods.
 */
public class System {
    /** Don't let anyone instantiate this class */
    private System(){};

    /**
     * The "standard" input stream. This stream is already
     * open and ready to supply input data. Typically this stream
     * corresponds to keyboard input or another input source specified by
     * the host environment or user.
     */
    public final static InputStream in = null;
    /**
     * The "standard" output stream. This stream is already
     * open and ready to accept output data. Typically this stream
     * corresponds to display output or another output destination
     * specified by the host environment or user.
     */
    public final static PrintStream out = null;
    /**
     * The "standard" error output stream. This stream is already
     * open and ready to accept output data.
     * <p>
     * Typically this stream corresponds to display output or another
     * output destination specified by the host environment or user. By
     * convention, this output stream is used to display error messages
     * or other information that should come to the immediate attention
     * of a user even if the principal output stream, the value of the
     * variable <code>out</code>, has been redirected to a file or other
     * destination that is typically not continuously monitored.
     */
    public final static PrintStream err = null;

    /**
     * Return time elapsed since startup of the Java Virtual Machine.
     * @return time in milliseconds
     */
    public native static long currentTimeMillis();
    /**
     * Invoke the garbage collector of the Java Virtual Machine
     */
    public native static void gc();
    /**
     * Return time elapsed since startup of the Java Virtual Machine.
     * @return time in nanoseconds
     */
    public native static long nanoTime();
    /**
     * Loads the native library specified by the <code>libname</code>
     */
    public native static void loadLibrary(String libname);
    /**
     * Copy a set of elements of an array to another.
     * @param src array to copy
     * @param srcPos index of the first element to copy
     * @param dst destination array
     * @param dstPos index of the first destination element
     * @param len number of elements to copy
     * @throws IndexOutOfBoundsException if srcPos, dstPos and len are not compatible with src and
     *     dst array
     * @throws ArrayStoreException if src elements type are not assignable to dst elements type or
     *     src or dst are not array type
     */
    public static native void arraycopy(
        Object src, int srcPos, Object dest, int destPos, int length);

    private static native void initializeStream();
    /**
     * Initialize the system class.  Called after thread initialization.
     */
    private static void initializeSystemClass() { initializeStream(); }
}
