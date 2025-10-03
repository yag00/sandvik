package java.lang;
/**
 * @@sandvik modified
 * The {@code Integer} class wraps a value of the primitive type
 * {@code int} in an object. An object of type {@code Integer}
 * contains a single field whose type is {@code int}.
 *
 * <p>In addition, this class provides several methods for converting
 * an {@code int} to a {@code String} and a {@code String} to an
 * {@code int}, as well as other constants and methods useful when
 * dealing with an {@code int}.
 */
public class Integer
    extends java.lang.Number implements java.io.Serializable, java.lang.Comparable<Integer> {
    private int val;
    /**
     * Maximum value of a integer
     */
    public final static int MAX_VALUE = 2147483647;
    /**
     * Minimum value of a integer
     */
    public final static int MIN_VALUE = -2147483648;
    /**
     * Size of a integer in bit
     */
    public final static int SIZE = 32;
    /**
     * Class representing the primitive type
     */
    public static java.lang.Class TYPE;

    // ---- IntegerCache ----
    private static class IntegerCache {
        static final int LOW = -128;
        static final int HIGH = 127;
        static final Integer[] cache;

        static {
            cache = new Integer[(HIGH - LOW) + 1];
            for (int i = 0; i < cache.length; i++) {
                cache[i] = new Integer(i + LOW);
            }
        }
    }

    /**
     * Create a Integer from an integer value.
     * @param val integer value
     */
    public Integer(int val) { this.val = val; }
    /**
     * Create a Integer from a string representation of a integer.
     * @param str integer value
     * @throws NumberFormatException if string does not represent an integer value
     */
    public Integer(java.lang.String str) throws NumberFormatException { this.val = parseInt(str); }
    /**
     * Return the number of bit at '1' in the integer.
     * @param val integer value
     * @return the bit count
     */
    public native final static int bitCount(int val);
    /**
     * Compare integer with another in a numerical ordering manner.
     * @param val integer value
     * @param other integer to compare to
     * @return negative value if integer is less than the other, 0 if integer is equal to other,
     *     positive value in other case
     */
    public native final static int compare(int val, int other);
    /**
     * Return the Integer corresponding to the string.
     * @param str string value
     * @return the Integer
     * @throws NumberFormatException if string does not represent a integer value
     */
    public native final static java.lang.Integer decode(java.lang.String str)
        throws NumberFormatException;
    /**
     * Return an integer with only the highest bit at '1' in the integer.
     * @param val integer value
     * @return the integer
     */
    public native final static int highestOneBit(int val);
    /**
     * Return an integer with only the lowest bit at '1' in the integer.
     * @param val integer value
     * @return the integer
     */
    public native final static int lowestOneBit(int val);
    /**
     * Return number of bits before the highest bit at '1' in the integer.
     * @param val integer value
     * @return the number of bits
     */
    public native final static int numberOfLeadingZeros(int val);
    /**
     * Return number of bits after the lowest bit at '1' in the integer.
     * @param val integer value
     * @return the number of bits
     */
    public native final static int numberOfTrailingZeros(int val);
    /**
     * Return the integer value corresponding to the string.
     * @param str string value
     * @return the integer value
     * @throws NumberFormatException if string does not represent a integer value
     */
    public native final static int parseInt(java.lang.String str) throws NumberFormatException;
    /**
     * Return the integer value corresponding to the string in a given base.
     * @param str string value
     * @param radix base value
     * @return the integer value
     * @throws NumberFormatException if string does not represent a integer value
     */
    public native final static int parseInt(java.lang.String str, int radix)
        throws NumberFormatException;
    /**
     * Invert each bit of the integer.
     * @param val integer value
     * @return the inverted integer
     */
    public native final static int reverse(int val);
    /**
     * Invert each byte of the integer.
     * @param val integer value
     * @return the inverted integer
     */
    public native final static int reverseBytes(int val);
    /**
     * Compute a left shift of each bit of the integer.
     * @param val integer value
     * @param shift number of bits to shift
     * @return the left shifted integer
     */
    public native final static int rotateLeft(int val, int shift);
    /**
     * Compute a right shift of each bit of the integer.
     * @param val integer value
     * @param shift number of bits to shift
     * @return the right shifted integer
     */
    public native final static int rotateRight(int val, int shift);
    /**
     * Retun the sign of the integer.
     * @param val integer value
     * @return -1 if negative, 1 if positive, 0 in other case
     */
    public native final static int signum(int val);
    /**
     * Return the binary string representation of the integer.
     * @param val integer value
     * @return string representation
     */
    public native final static java.lang.String toBinaryString(int val);
    /**
     * Return the octal string representation of the integer.
     * @param val integer value
     * @return string representation
     */
    public native final static java.lang.String toOctalString(int val);
    /**
     * Return the hexadecimal string representation of the integer.
     * @param val integer value
     * @return string representation
     */
    public native final static java.lang.String toHexString(int val);
    /**
     * Return the decimal string representation of the integer.
     * @param val integer value
     * @return string representation
     */
    public native final static java.lang.String toString(int val);
    /**
     * Return the decimal string representation of the integer in a given base.
     * @param val integer value
     * @param radix base value
     * @return string representation
     */
    public native final static java.lang.String toString(int val, int radix);
    /**
     * Return the unsigned integer value of the integer.
     * @param val integer value
     * @return unsigned value
     */
    public native final static long toUnsignedLong(int val);
    /**
     * Return the decimal string representation of the unsigned integer.
     * @param val integer value
     * @return string representation
     */
    public native final static java.lang.String toUnsignedString(int val);
    /**
     * Return the decimal string representation of the unsigned integer in a given base.
     * @param val integer value
     * @param radix base value
     * @return string representation
     */
    public native final static java.lang.String toUnsignedString(int val, int radix);
    /**
     * Return the Integer corresponding to the integer value.
     * @param val integer value
     * @return the Integer
     */
    public final static java.lang.Integer valueOf(int val) {
        if (val >= IntegerCache.LOW && val <= IntegerCache.HIGH) {
            return IntegerCache.cache[val - IntegerCache.LOW];
        }
        return new Integer(val);
    }
    /**
     * Return the Integer corresponding to the string.
     * @param str string value
     * @return the Integer
     * @throws NumberFormatException if string does not represent a integer value
     */
    public native final static java.lang.Integer valueOf(java.lang.String str)
        throws NumberFormatException;
    /**
     * Return the Integer corresponding to the string in a given base.
     * @param str string value
     * @param radix base value
     * @return the Integer
     * @throws NumberFormatException if string does not represent a integer value
     */
    public native final static java.lang.Integer valueOf(java.lang.String str, int radix)
        throws NumberFormatException;
    /**
     * Return the byte value of the Integer.
     * @return the value
     */
    public native final byte byteValue();
    /**
     * Compare the Integer with another in a numerical ordering manner.
     * @param other integer to compare to
     * @return negative value if integer is less than the other, 0 if integer is equal to other,
     *     positive value in other case
     */
    public native final int compareTo(Integer other);
    /**
     * Return the double value of the Integer.
     * @return the value
     */
    public native final double doubleValue();
    /**
     * Indicates if the Integer is equal to a given one.
     * @param obj object to check
     * @return true if the Integer is equal to the object
     */
    public native final boolean equals(java.lang.Object obj);
    /**
     * Return the float value of the Integer.
     * @return the value
     */
    public native final float floatValue();
    /**
     * Return the hashCode of the Integer.
     * @return hashCode value
     */
    public native final int hashCode();
    /**
     * Return the integer value of the Integer.
     * @return the value
     */
    public final int intValue() { return val; }
    /**
     * Return the long value of the Integer.
     * @return the value
     */
    public native final long longValue();
    /**
     * Return the sort value of the Integer.
     * @return the value
     */
    public native final short shortValue();
    /**
     * Return the string representation of the Integer.
     * @return string representation
     */
    public native final java.lang.String toString();
}
