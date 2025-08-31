package java.lang;
/**
 * @@sandvik modified
 * The {@code Double} class wraps a value of the primitive type
 * {@code double} in an object. An object of type
 * {@code Double} contains a single field whose type is
 * {@code double}.
 *
 * <p>In addition, this class provides several methods for converting a
 * {@code double} to a {@code String} and a
 * {@code String} to a {@code double}, as well as other
 * constants and methods useful when dealing with a
 * {@code double}.
 */
public final class Double extends Number implements Comparable<Double> {
    private double val;
    /**
     * Maximum value of a double
     */
    public static double MAX_VALUE;
    /**
     * Minimum value of a denormalized double
     */
    public static double MIN_VALUE;
    /**
     * Minimum value of a normalized double
     */
    public static double MIN_NORMAL;
    /**
     * Value of a Not a Number double
     */
    public static double NaN;
    /**
     * Value of a negative infinite double
     */
    public static double NEGATIVE_INFINITY;
    /**
     * Value of a positive infinite double
     */
    public static double POSITIVE_INFINITY;
    /**
     * Size of a double in bit
     */
    public final static int SIZE = 64;
    /**
     * Class representing the primitive type
     */
    public static java.lang.Class TYPE;
    /**
     * Compare double with another in a numerical ordering manner.
     * @param val double value
     * @param other double to compare to
     * @return negative value if double is less than the other, 0 if double is equal to other,
     *     positive value in other case
     */
    public native final static int compare(double val, double other);
    /**
     * Return the raw value of the double.
     * @param val double value
     * @return raw value
     */
    public native final static long doubleToLongBits(double val);
    /**
     * Return the raw value of the double.
     * @param val double value
     * @return raw value
     */
    public native final static long doubleToRawLongBits(double val);
    /**
     * Indicates if double is infinite.
     * @param val double value
     * @return true if infinite
     */
    public native final static boolean isInfinite(double val);
    /**
     * Indicates if double is Not a Number.
     * @param val double value
     * @return true if Not a Number
     */
    public native final static boolean isNaN(double val);
    /**
     * Return the double value corresponding to the given raw value.
     * @param val raw value
     * @return double value
     */
    public native final static double longBitsToDouble(long val);
    /**
     * Return the double value corresponding to the string.
     * @param str string value
     * @return the double value
     * @throws NumberFormatException if string does not represent a double value
     */
    public native final static double parseDouble(java.lang.String str)
        throws NumberFormatException;
    /**
     * Return the string representation of the double value.
     * @param val double value
     * @return string representation
     */
    public native final static java.lang.String toString(double val);
    /**
     * Return the Double corresponding to the double value.
     * @param val double value
     * @return the Double
     */
    public native final static java.lang.Double valueOf(double val);
    /**
     * Return the Double corresponding to the string.
     * @param str string value
     * @return the Double
     * @throws NumberFormatException if string does not represent a double value
     */
    public native final static java.lang.Double valueOf(java.lang.String str)
        throws NumberFormatException;
    /**
     * Create a Double from a double value.
     * @param val double value
     */
    public Double(double val) {}
    /**
     * Create a Double from a string representation of a double.
     * @param str double value
     * @throws NumberFormatException if string does not represent a double value
     */
    public Double(java.lang.String str) throws NumberFormatException {}
    /**
     * Return the byte value of the Double.
     * @return the value
     */
    public native final byte byteValue();
    /**
     * Compare the Double with another in a numerical ordering manner.
     * @param other double to compare to
     * @return negative value if double is less than the other, 0 if double is equal to other,
     *     positive value in other case
     */
    public native final int compareTo(Double other);
    /**
     * Return the double value of the Double.
     * @return the value
     */
    public native final double doubleValue();
    /**
     * Indicates if the Double is equal to a given one.
     * @param obj object to check
     * @return true if the Double is equal to the object
     */
    public native final boolean equals(java.lang.Object obj);
    /**
     * Return the float value of the Double.
     * @return the value
     */
    public native final float floatValue();
    /**
     * Return the hashCode of the Double.
     * @return hashCode value
     */
    public native final int hashCode();
    /**
     * Return the integer value of the Double.
     * @return the value
     */
    public native final int intValue();
    /**
     * Indicates if Double is infinite.
     * @return true if infinite
     */
    public native final boolean isInfinite();
    /**
     * Indicates if Double is Not a Number.
     * @return true if Not a Number
     */
    public native final boolean isNaN();
    /**
     * Return the long value of the Double.
     * @return the value
     */
    public native final long longValue();
    /**
     * Return the sort value of the Double.
     * @return the value
     */
    public native final short shortValue();
    /**
     * Return the string representation of the Double.
     * @return string representation
     */
    public native final java.lang.String toString();
}
