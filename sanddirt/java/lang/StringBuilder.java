package java.lang;
/**
 * @@sandvik modified
 */
public class StringBuilder implements java.io.Serializable, java.lang.CharSequence {
    private java.lang.String string;
    private int len;
    /**
     * Create an empty StringBuilder
     */
    public StringBuilder() {}
    /**
     * Create a StringBuilder with the specified string.
     * @param str initial string
     */
    public StringBuilder(java.lang.String str) { string = str; }
    /**
     * Create an empty StringBuilder with an initial capacity.
     * @param capacity initial capacity
     */
    public StringBuilder(int capacity) {}
    /**
     * Append a boolean in the StringBuilder.
     * @param val boolean value
     * @return the StringBuilder
     */
    public native final java.lang.StringBuilder append(boolean val);
    /**
     * Append a character in the StringBuilder.
     * @param val character value
     * @return the StringBuilder
     */
    public native final java.lang.StringBuilder append(char val);
    /**
     * Append an array of char in the StringBuilder.
     * @param val array of char
     * @return the StringBuilder
     */
    public native final java.lang.StringBuilder append(char[] val);
    /**
     * Append a part of an array of char in the StringBuilder.
     * @param val array of char
     * @param offset index of the first element
     * @param count number of element to add
     * @return the StringBuilder
     */
    public native final java.lang.StringBuilder append(char[] val, int offset, int count);
    /**
     * Append a CharSequence in the StringBuilder.
     * @param val CharSequence value
     * @return the StringBuilder
     */
    public native final java.lang.StringBuilder append(java.lang.CharSequence val);
    /**
     * Append a part of a CharSequence in the StringBuilder.
     * @param val CharSequence value
     * @param from index of the first element
     * @param to index following the last element to add
     * @return the StringBuilder
     */
    public native final java.lang.StringBuilder append(java.lang.CharSequence val,
                                                       int from,
                                                       int to);
    /**
     * Append a double in the StringBuilder.
     * @param val double value
     * @return the StringBuilder
     */
    public native final java.lang.StringBuilder append(double val);
    /**
     * Append a float in the StringBuilder.
     * @param val float value
     * @return the StringBuilder
     */
    public native final java.lang.StringBuilder append(float val);
    /**
     * Append an integer in the StringBuilder.
     * @param val integer value
     * @return the StringBuilder
     */
    public native final java.lang.StringBuilder append(int val);
    /**
     * Append a long in the StringBuilder.
     * @param val long value
     * @return the StringBuilder
     */
    public native final java.lang.StringBuilder append(long val);
    /**
     * Append a representation of an object in the StringBuilder.
     * @param val object value
     * @return the StringBuilder
     */
    public native final java.lang.StringBuilder append(java.lang.Object val);
    /**
     * Append a string in the StringBuilder.
     * @param val string value
     * @return the StringBuilder
     */
    public native final java.lang.StringBuilder append(java.lang.String val);
    /**
     * Append a stringBuffer in the StringBuilder.
     * @param val stringBuffer value
     * @return the StringBuilder
     */
    public native final java.lang.StringBuilder append(java.lang.StringBuffer val);
    /**
     * Append a char represented by a codePoint in the StringBuilder.
     * @param val stricodePointng value
     * @return the StringBuilder
     */
    public native final java.lang.StringBuilder appendCodePoint(int val);
    /**
     * Return the character at the specified index.
     * @param pos index in the StringBuilder
     * @return the character
     */
    public native final char charAt(int pos);
    /**
     * Return the character'codePoint at the specified index.
     * @param pos index in the StringBuilder
     * @return the codePoint
     */
    public native final int codePointAt(int pos);
    /**
     * Return the character'codePoint before the specified index.
     * @param pos index in the StringBuilder
     * @return the codePoint
     */
    public native final int codePointBefore(int pos);
    /**
     * Return the number of codePoint in the specified range.
     * In this implementation, simply return {@code to-from}.
     * @param from index of the first element
     * @param to index following the last element
     * @return the number of codePoint
     */
    public native final int codePointCount(int from, int to);
    /**
     * Delete a part of the StringBuilder specified by a range.
     * @param from index of the first element
     * @param to index following the last element
     * @return the StringBuilder
     */
    public native final java.lang.StringBuilder delete(int from, int to);
    /**
     * Return the first index of a given string in the StringBuilder.
     * @param str string to search
     * @return index of the string in the StringBuilder or -1 if not found
     */
    public native final int indexOf(java.lang.String str);
    /**
     * Return the first index of a given string in the StringBuilder beginning at a given index.
     * @param str string to search
     * @param pos index of first element
     * @return index of the string in the StringBuilder or -1 if not found
     */
    public native final int indexOf(java.lang.String str, int pos);
    /**
     * Return the last index of a given string in the StringBuilder.
     * @param str string to search
     * @return index of the string in the StringBuilder or -1 if not found
     */
    public native final int lastIndexOf(java.lang.String str);
    /**
     * Return the last index of a given string in the StringBuilder beginning at a given index.
     * @param str string to search
     * @param pos index of first element
     * @return index of the string in the StringBuilder or -1 if not found
     */
    public native final int lastIndexOf(java.lang.String str, int pos);
    /**
     * Return the length of the StringBuilder.
     * @return length
     */
    public native final int length();
    /**
     * Set the length of the StringBuilder.
     * Add 'null' characters or truncate StringBuilder if need
     * @param length length to set
     */
    public native final void setLength(int length);
    /**
     * Return a part of the StringBuilder beginning at a given position
     * @param pos index of the first element
     * @return a new string beginning at pos index
     */
    public native final java.lang.String substring(int pos);
    /**
     * Return a part of the StringBuilder specified by a range
     * @param from index of the first element
     * @param to index following the last element
     * @return a new string
     */
    public native final java.lang.String substring(int from, int to);
    /**
     * Return a part of the StringBuilder specified by a range
     * @param from index of the first element
     * @param to index following the last element
     * @return a new CharSequence (in this implementation a string)
     */
    public native final java.lang.CharSequence subSequence(int from, int to);
    /**
     * Return the string representation of the StringBuilder.
     * @return string representation
     */
    public native final java.lang.String toString();

    public native StringBuilder insert(int index, char[] str, int offset, int len);
    public native StringBuilder insert(int offset, Object obj);
    public native StringBuilder insert(int offset, String str);
    public native StringBuilder insert(int offset, char[] str);
    public native StringBuilder insert(int dstOffset, CharSequence s);
    public native StringBuilder insert(int dstOffset, CharSequence s, int start, int end);
    public native StringBuilder insert(int offset, boolean b);
    public native StringBuilder insert(int offset, char c);
    public native StringBuilder insert(int offset, int i);
    public native StringBuilder insert(int offset, long l);
    public native StringBuilder insert(int offset, float f);
}
