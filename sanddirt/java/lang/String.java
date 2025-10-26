package java.lang;
import java.util.Locale;
/**
 * @@sandvik modified
 * The {@code String} class represents character strings. All
 * string literals in Java programs, such as {@code "abc"}, are
 * implemented as instances of this class.
 */
public class String
    implements java.io.Serializable, java.lang.Comparable<String>, java.lang.CharSequence {
    private native void initialize(java.lang.String str);
    private native void initialize(byte[] val);
    private native void initialize(byte[] val, int offset, int count);
    private native void initialize(char[] val);
    private native void initialize(char[] val, int offset, int count);
    private native void initialize(int[] val, int offset, int count);
    private native void initialize(byte[] val, int hibyte, int offset, int count);
    private native void initialize(byte[] val, int offset, int count, java.lang.String charSet);
    private native void initialize(java.lang.StringBuffer str);
    private native void initialize(java.lang.StringBuilder str);
    /**
     * Create an empty string
     */
    public String() { initialize(""); }
    /**
     * Create a copy of a given string.
     * @param str the string to copy
     */
    public String(java.lang.String str) { initialize(str); }
    /**
     * Create a string from an array of byte.
     * @param val array of byte
     */
    public String(byte[] val) { initialize(val); }
    /**
     * Create a string from a part of array of byte.
     * @param val array of byte
     * @param offset index of the first byte
     * @param count number of byte to copy
     */
    public String(byte[] val, int offset, int count) { initialize(val, offset, count); }
    /**
     * Create a string from an array of char.
     * @param val array of char
     */
    public String(char[] val) { initialize(val); }
    /**
     * Create a string from a part of array of char.
     * @param val array of char
     * @param offset index of the first char
     * @param count number of char to copy
     */
    public String(char[] val, int offset, int count) { initialize(val, offset, count); }
    /**
     * Create a string from a part of array of codePoint.
     * @param val array of codePoint
     * @param offset index of the first codePoint
     * @param count number of codePoint to copy
     */
    public String(int[] val, int offset, int count) { initialize(val, offset, count); }
    /**
     * Create a string from a part of array of byte.
     * @param val array of byte
     * @param hibyte not taken into account
     * @param offset index of the first byte
     * @param count number of byte to copy
     */
    public String(byte[] val, int hibyte, int offset, int count) {
        initialize(val, hibyte, offset, count);
    }
    /**
     * Create a string from a part of array of byte.
     * @param val array of byte
     * @param offset index of the first byte
     * @param count number of byte to copy
     * @param charSet not taken into account
     */
    public String(byte[] val, int offset, int count, java.lang.String charSet) {
        initialize(val, offset, count, charSet);
    }
    /**
     * Create a copy of a given string.
     * @param str the string to copy
     */
    public String(java.lang.StringBuffer str) { initialize(str); }
    /**
     * Create a copy of a given string.
     * @param str the string to copy
     */
    public String(java.lang.StringBuilder str) { initialize(str); }
    /*
     * Package private constructor which shares value array for speed.
     * this constructor is always expected to be called with share==true.
     * a separate constructor is needed because we already have a public
     * String(char[]) constructor that makes a copy of the given char[].
     */
    String(char[] value, boolean share) { initialize(value); }
    /**
     * Return the character at the specified index.
     * @param pos index in the String
     * @return the character
     */
    public native final char charAt(int pos);
    /**
     * Return the character'codePoint at the specified index.
     * @param pos index in the String
     * @return the codePoint
     */
    public native final int codePointAt(int pos);
    /**
     * Return the character'codePoint before the specified index.
     * @param pos index in the String
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
     * Compare string with another in a lexical ordering manner.
     * @param other string to compare to
     * @return negative value if string is less than the other, 0 if string is equal to other,
     *     positive value in other case
     */
    public native final int compareTo(String other);
    /**
     * Create a new string the concatenation of another.
     * @param str string to concatenate
     * @return the new string
     */
    public native final java.lang.String concat(java.lang.String str);
    /**
     * Indicates if the end of the string is equal to a given one.
     * @param str string to check
     * @return true if the string ends with {@code str}
     */
    public native final boolean endsWith(java.lang.String str);
    /**
     * Indicates if the string is equal to a given one.
     * @param obj object to check
     * @return true if the string is equal to the object
     */
    public native final boolean equals(java.lang.Object obj);
    /**
     * Indicates if the string is equal to a given one ignoring upper or lower case.
     * @param obj object to check
     * @return true if the string is equal to the object
     */
    public native final boolean equalsIgnoreCase(java.lang.String obj);
    /**
     * Copy characters from this string into dst starting at dstBegin.
     * This method doesn't perform any range checking.
     */
    native void getChars(char dst[], int dstBegin);

    /**
     * Copies characters from this string into the destination character
     * array.
     * <p>
     * The first character to be copied is at index {@code srcBegin};
     * the last character to be copied is at index {@code srcEnd-1}
     * (thus the total number of characters to be copied is
     * {@code srcEnd-srcBegin}). The characters are copied into the
     * subarray of {@code dst} starting at index {@code dstBegin}
     * and ending at index:
     * <blockquote><pre>
     *     dstbegin + (srcEnd-srcBegin) - 1
     * </pre></blockquote>
     *
     * @param      srcBegin   index of the first character in the string
     *                        to copy.
     * @param      srcEnd     index after the last character in the string
     *                        to copy.
     * @param      dst        the destination array.
     * @param      dstBegin   the start offset in the destination array.
     * @exception IndexOutOfBoundsException If any of the following
     *            is true:
     *            <ul><li>{@code srcBegin} is negative.
     *            <li>{@code srcBegin} is greater than {@code srcEnd}
     *            <li>{@code srcEnd} is greater than the length of this
     *                string
     *            <li>{@code dstBegin} is negative
     *            <li>{@code dstBegin+(srcEnd-srcBegin)} is larger than
     *                {@code dst.length}</ul>
     */
    public native void getChars(int srcBegin, int srcEnd, char dst[], int dstBegin);
    /**
     * Return an array of bytes representing the string.
     * @return array of bytes
     */
    public native final byte[] getBytes();
    /**
     * Return the hashCode of the string.
     * @return hashCode value
     */
    public native final int hashCode();

    /**
     * Returns the index within this string of the first occurrence of
     * the specified character. If a character with value
     * {@code ch} occurs in the character sequence represented by
     * this {@code String} object, then the index (in Unicode
     * code units) of the first such occurrence is returned. For
     * values of {@code ch} in the range from 0 to 0xFFFF
     * (inclusive), this is the smallest value <i>k</i> such that:
     * <blockquote><pre>
     * this.charAt(<i>k</i>) == ch
     * </pre></blockquote>
     * is true. For other values of {@code ch}, it is the
     * smallest value <i>k</i> such that:
     * <blockquote><pre>
     * this.codePointAt(<i>k</i>) == ch
     * </pre></blockquote>
     * is true. In either case, if no such character occurs in this
     * string, then {@code -1} is returned.
     *
     * @param   ch   a character (Unicode code point).
     * @return  the index of the first occurrence of the character in the
     *          character sequence represented by this object, or
     *          {@code -1} if the character does not occur.
     */
    public int indexOf(int ch) { return indexOf(ch, 0); }
    /**
     * Returns the index within this string of the first occurrence of the
     * specified character, starting the search at the specified index.
     * <p>
     * If a character with value {@code ch} occurs in the
     * character sequence represented by this {@code String}
     * object at an index no smaller than {@code fromIndex}, then
     * the index of the first such occurrence is returned. For values
     * of {@code ch} in the range from 0 to 0xFFFF (inclusive),
     * this is the smallest value <i>k</i> such that:
     * <blockquote><pre>
     * (this.charAt(<i>k</i>) == ch) {@code &&} (<i>k</i> &gt;= fromIndex)
     * </pre></blockquote>
     * is true. For other values of {@code ch}, it is the
     * smallest value <i>k</i> such that:
     * <blockquote><pre>
     * (this.codePointAt(<i>k</i>) == ch) {@code &&} (<i>k</i> &gt;= fromIndex)
     * </pre></blockquote>
     * is true. In either case, if no such character occurs in this
     * string at or after position {@code fromIndex}, then
     * {@code -1} is returned.
     *
     * <p>
     * There is no restriction on the value of {@code fromIndex}. If it
     * is negative, it has the same effect as if it were zero: this entire
     * string may be searched. If it is greater than the length of this
     * string, it has the same effect as if it were equal to the length of
     * this string: {@code -1} is returned.
     *
     * <p>All indices are specified in {@code char} values
     * (Unicode code units).
     *
     * @param   ch          a character (Unicode code point).
     * @param   fromIndex   the index to start the search from.
     * @return  the index of the first occurrence of the character in the
     *          character sequence represented by this object that is greater
     *          than or equal to {@code fromIndex}, or {@code -1}
     *          if the character does not occur.
     */
    public native int indexOf(int ch, int fromIndex);
    /**
     * Returns the index within this string of the last occurrence of
     * the specified character. For values of {@code ch} in the
     * range from 0 to 0xFFFF (inclusive), the index (in Unicode code
     * units) returned is the largest value <i>k</i> such that:
     * <blockquote><pre>
     * this.charAt(<i>k</i>) == ch
     * </pre></blockquote>
     * is true. For other values of {@code ch}, it is the
     * largest value <i>k</i> such that:
     * <blockquote><pre>
     * this.codePointAt(<i>k</i>) == ch
     * </pre></blockquote>
     * is true.  In either case, if no such character occurs in this
     * string, then {@code -1} is returned.  The
     * {@code String} is searched backwards starting at the last
     * character.
     *
     * @param   ch   a character (Unicode code point).
     * @return  the index of the last occurrence of the character in the
     *          character sequence represented by this object, or
     *          {@code -1} if the character does not occur.
     */
    public int lastIndexOf(int ch) { return lastIndexOf(ch, length() - 1); }

    /**
     * Returns the index within this string of the last occurrence of
     * the specified character, searching backward starting at the
     * specified index. For values of {@code ch} in the range
     * from 0 to 0xFFFF (inclusive), the index returned is the largest
     * value <i>k</i> such that:
     * <blockquote><pre>
     * (this.charAt(<i>k</i>) == ch) {@code &&} (<i>k</i> &lt;= fromIndex)
     * </pre></blockquote>
     * is true. For other values of {@code ch}, it is the
     * largest value <i>k</i> such that:
     * <blockquote><pre>
     * (this.codePointAt(<i>k</i>) == ch) {@code &&} (<i>k</i> &lt;= fromIndex)
     * </pre></blockquote>
     * is true. In either case, if no such character occurs in this
     * string at or before position {@code fromIndex}, then
     * {@code -1} is returned.
     *
     * <p>All indices are specified in {@code char} values
     * (Unicode code units).
     *
     * @param   ch          a character (Unicode code point).
     * @param   fromIndex   the index to start the search from. There is no
     *          restriction on the value of {@code fromIndex}. If it is
     *          greater than or equal to the length of this string, it has
     *          the same effect as if it were equal to one less than the
     *          length of this string: this entire string may be searched.
     *          If it is negative, it has the same effect as if it were -1:
     *          -1 is returned.
     * @return  the index of the last occurrence of the character in the
     *          character sequence represented by this object that is less
     *          than or equal to {@code fromIndex}, or {@code -1}
     *          if the character does not occur before that point.
     */
    public native int lastIndexOf(int ch, int fromIndex);

    /**
     * Returns the index within this string of the first occurrence of the
     * specified substring.
     *
     * <p>The returned index is the smallest value <i>k</i> for which:
     * <blockquote><pre>
     * this.startsWith(str, <i>k</i>)
     * </pre></blockquote>
     * If no such value of <i>k</i> exists, then {@code -1} is returned.
     *
     * @param   str   the substring to search for.
     * @return  the index of the first occurrence of the specified substring,
     *          or {@code -1} if there is no such occurrence.
     */
    public int indexOf(String str) { return indexOf(str, 0); }

    /**
     * Returns the index within this string of the first occurrence of the
     * specified substring, starting at the specified index.
     *
     * <p>The returned index is the smallest value <i>k</i> for which:
     * <blockquote><pre>
     * <i>k</i> &gt;= fromIndex {@code &&} this.startsWith(str, <i>k</i>)
     * </pre></blockquote>
     * If no such value of <i>k</i> exists, then {@code -1} is returned.
     *
     * @param   str         the substring to search for.
     * @param   fromIndex   the index from which to start the search.
     * @return  the index of the first occurrence of the specified substring,
     *          starting at the specified index,
     *          or {@code -1} if there is no such occurrence.
     */
    public native int indexOf(String str, int fromIndex);

    /**
     * Code shared by String and AbstractStringBuilder to do searches. The
     * source is the character array being searched, and the target
     * is the string being searched for.
     *
     * @param   source       the characters being searched.
     * @param   sourceOffset offset of the source string.
     * @param   sourceCount  count of the source string.
     * @param   target       the characters being searched for.
     * @param   fromIndex    the index to begin searching from.
     */
    static native int indexOf(
        char[] source, int sourceOffset, int sourceCount, String target, int fromIndex);

    /**
     * Code shared by String and StringBuffer to do searches. The
     * source is the character array being searched, and the target
     * is the string being searched for.
     *
     * @param   source       the characters being searched.
     * @param   sourceOffset offset of the source string.
     * @param   sourceCount  count of the source string.
     * @param   target       the characters being searched for.
     * @param   targetOffset offset of the target string.
     * @param   targetCount  count of the target string.
     * @param   fromIndex    the index to begin searching from.
     */
    static int indexOf(char[] source,
                       int sourceOffset,
                       int sourceCount,
                       char[] target,
                       int targetOffset,
                       int targetCount,
                       int fromIndex) {
        if (fromIndex >= sourceCount) {
            return (targetCount == 0 ? sourceCount : -1);
        }
        if (fromIndex < 0) {
            fromIndex = 0;
        }
        if (targetCount == 0) {
            return fromIndex;
        }

        char first = target[targetOffset];
        int max = sourceOffset + (sourceCount - targetCount);

        for (int i = sourceOffset + fromIndex; i <= max; i++) {
            /* Look for first character. */
            if (source[i] != first) {
                while (++i <= max && source[i] != first)
                    ;
            }

            /* Found first character, now look at the rest of v2 */
            if (i <= max) {
                int j = i + 1;
                int end = j + targetCount - 1;
                for (int k = targetOffset + 1; j < end && source[j] == target[k]; j++, k++)
                    ;

                if (j == end) {
                    /* Found whole string. */
                    return i - sourceOffset;
                }
            }
        }
        return -1;
    }

    /**
     * Returns the index within this string of the last occurrence of the
     * specified substring.  The last occurrence of the empty string ""
     * is considered to occur at the index value {@code this.length()}.
     *
     * <p>The returned index is the largest value <i>k</i> for which:
     * <blockquote><pre>
     * this.startsWith(str, <i>k</i>)
     * </pre></blockquote>
     * If no such value of <i>k</i> exists, then {@code -1} is returned.
     *
     * @param   str   the substring to search for.
     * @return  the index of the last occurrence of the specified substring,
     *          or {@code -1} if there is no such occurrence.
     */
    public int lastIndexOf(String str) { return lastIndexOf(str, length()); }

    /**
     * Returns the index within this string of the last occurrence of the
     * specified substring, searching backward starting at the specified index.
     *
     * <p>The returned index is the largest value <i>k</i> for which:
     * <blockquote><pre>
     * <i>k</i> {@code <=} fromIndex {@code &&} this.startsWith(str, <i>k</i>)
     * </pre></blockquote>
     * If no such value of <i>k</i> exists, then {@code -1} is returned.
     *
     * @param   str         the substring to search for.
     * @param   fromIndex   the index to start the search from.
     * @return  the index of the last occurrence of the specified substring,
     *          searching backward from the specified index,
     *          or {@code -1} if there is no such occurrence.
     */
    public native int lastIndexOf(String str, int fromIndex);

    /**
     * Code shared by String and AbstractStringBuilder to do searches. The
     * source is the character array being searched, and the target
     * is the string being searched for.
     *
     * @param   source       the characters being searched.
     * @param   sourceOffset offset of the source string.
     * @param   sourceCount  count of the source string.
     * @param   target       the characters being searched for.
     * @param   fromIndex    the index to begin searching from.
     */
    static native int lastIndexOf(
        char[] source, int sourceOffset, int sourceCount, String target, int fromIndex);

    /**
     * Code shared by String and StringBuffer to do searches. The
     * source is the character array being searched, and the target
     * is the string being searched for.
     *
     * @param   source       the characters being searched.
     * @param   sourceOffset offset of the source string.
     * @param   sourceCount  count of the source string.
     * @param   target       the characters being searched for.
     * @param   targetOffset offset of the target string.
     * @param   targetCount  count of the target string.
     * @param   fromIndex    the index to begin searching from.
     */
    static int lastIndexOf(char[] source,
                           int sourceOffset,
                           int sourceCount,
                           char[] target,
                           int targetOffset,
                           int targetCount,
                           int fromIndex) {
        /*
         * Check arguments; return immediately where possible. For
         * consistency, don't check for null str.
         */
        int rightIndex = sourceCount - targetCount;
        if (fromIndex < 0) {
            return -1;
        }
        if (fromIndex > rightIndex) {
            fromIndex = rightIndex;
        }
        /* Empty string always matches. */
        if (targetCount == 0) {
            return fromIndex;
        }

        int strLastIndex = targetOffset + targetCount - 1;
        char strLastChar = target[strLastIndex];
        int min = sourceOffset + targetCount - 1;
        int i = min + fromIndex;

    startSearchForLastChar:
        while (true) {
            while (i >= min && source[i] != strLastChar) {
                i--;
            }
            if (i < min) {
                return -1;
            }
            int j = i - 1;
            int start = j - (targetCount - 1);
            int k = strLastIndex - 1;

            while (j > start) {
                if (source[j--] != target[k--]) {
                    i--;
                    continue startSearchForLastChar;
                }
            }
            return start - sourceOffset + 1;
        }
    }
    /**
     * Return the length of the String.
     * @return length
     */
    public native final int length();
    /**
     * Returns {@code true} if, and only if, {@link #length()} is {@code 0}.
     *
     * @return {@code true} if {@link #length()} is {@code 0}, otherwise
     * {@code false}
     *
     * @since 1.6
     */
    public boolean isEmpty() { return length() == 0; }
    /**
     * Indicates if the string matches a given string pattern.
     * It is necessary to load java library to use this method.
     * @param pattern string pattern to check
     * @return true if the string matches the pattern
     */
    public native final boolean matches(java.lang.String pattern);
    /**
     * Return a new string by replacing a character by another.
     * @param old character to replace
     * @param val the character replacing {@code old}
     * @return the new string
     */
    public native final java.lang.String replace(char old, char val);
    /**
     * Return a new string by replacing a string by another.
     * @param old string to replace
     * @param val the string replacing {@code old}
     * @return the new string
     */
    public native final java.lang.String replace(java.lang.CharSequence old,
                                                 java.lang.CharSequence val);
    /**
     * Return a new string by replacing all string pattern by another.
     * It is necessary to load java library to use this method
     * @param pattern string pattern to replace
     * @param val the string replacing {@code pattern}
     * @return the new string
     */
    public native final java.lang.String replaceAll(java.lang.String pattern, java.lang.String val);
    /**
     * Return a new string by replacing the first string pattern by another.
     * It is necessary to load java library to use this method
     * @param pattern string pattern to replace
     * @param val the string replacing {@code pattern}
     * @return the new string
     */
    public native final java.lang.String replaceFirst(java.lang.String pattern,
                                                      java.lang.String val);
    /**
     * Indicates if the beginning of the string is equal to a given one.
     * @param str string to check
     * @return true if the string starts with {@code str}
     */
    public native final boolean startsWith(java.lang.String str);
    /**
     * Indicates if the beginning (at a given position) of the string is equal to a given one.
     * @param str string to check
     * @param pos index of first element
     * @return true if the string starts with {@code str}
     */
    public native final boolean startsWith(java.lang.String str, int pos);
    /**
     * Split the string by a given pattern and build an array with the elements.
     * It is necessary to load java library to use this method
     * @param pattern string pattern
     * @param limit number max of elements in array
     * @return the array
     */
    public native final java.lang.String[] split(java.lang.String pattern, int limit);
    /**
     * Split the string by a given pattern and build an array with the elements.
     * It is necessary to load java library to use this method
     * @param pattern string pattern
     * @return the array
     */
    public native final java.lang.String[] split(java.lang.String pattern);
    /**
     * Return a part of the String beginning at a given position
     * @param pos index of the first element
     * @return a new string beginning at pos index
     */
    public native final java.lang.String substring(int pos);
    /**
     * Return a part of the String specified by a range
     * @param from index of the first element
     * @param to index following the last element
     * @return a new string
     */
    public native final java.lang.String substring(int from, int to);
    /**
     * Return a part of the String specified by a range
     * @param from index of the first element
     * @param to index following the last element
     * @return a new CharSequence (in this implementation a string)
     */
    public native final java.lang.CharSequence subSequence(int from, int to);
    /**
     * Return a string without space before and after the string.
     * @return a new string
     */
    public native final java.lang.String trim();
    /**
     * Return an array of char representing the string.
     * @return array of char
     */
    public native final char[] toCharArray();
    /**
     * Return a copy of string with lower case character.
     * @return a lower cased string
     */
    public native final java.lang.String toLowerCase();
    /**
     * Return the string.
     * @return the same string
     */
    public native final java.lang.String toString();
    /**
     * Return a copy of string with upper case character.
     * @return a upper cased string
     */
    public native final java.lang.String toUpperCase(Locale locale);
    /**
     * Create a string from an array of char.
     * @param val array of char
     * @return the string
     */
    public native final static java.lang.String copyValueOf(char[] val);
    /**
     * Create a string from a part of array of char.
     * @param val array of char
     * @param offset index of the first char
     * @param count number of char to copy
     * @return the string
     */
    public native final static java.lang.String copyValueOf(char[] val, int offset, int count);
    /**
     * Create a string representation of a boolean value.
     * @param val boolean value
     * @return the string
     */
    public native final static java.lang.String valueOf(boolean val);
    /**
     * Create a string representation of a char value.
     * @param val char value
     * @return the string
     */
    public native final static java.lang.String valueOf(char val);
    /**
     * Create a string from an array of char.
     * @param val array of char
     * @return the string
     */
    public native final static java.lang.String valueOf(char[] val);
    /**
     * Create a string from a part of array of char.
     * @param val array of char
     * @param offset index of the first char
     * @param count number of char to copy
     * @return the string
     */
    public native final static java.lang.String valueOf(char[] val, int offset, int count);
    /**
     * Create a string representation of a double value.
     * @param val double value
     * @return the string
     */
    public native final static java.lang.String valueOf(double val);
    /**
     * Create a string representation of a float value.
     * @param val float value
     * @return the string
     */
    public native final static java.lang.String valueOf(float val);
    /**
     * Create a string representation of an integer value.
     * @param val integer value
     * @return the string
     */
    public native final static java.lang.String valueOf(int val);
    /**
     * Create a string representation of a long value.
     * @param val long value
     * @return the string
     */
    public native final static java.lang.String valueOf(long val);
    /**
     * Create a string representation of an object.
     * @param obj object
     * @return the string
     */
    public native final static java.lang.String valueOf(java.lang.Object obj);
    /**
     * Returns a canonical representation for the string object.
     * <p>
     * A pool of strings, initially empty, is maintained privately by the
     * class {@code String}.
     * <p>
     * When the intern method is invoked, if the pool already contains a
     * string equal to this {@code String} object as determined by
     * the {@link #equals(Object)} method, then the string from the pool is
     * returned. Otherwise, this {@code String} object is added to the
     * pool and a reference to this {@code String} object is returned.
     * <p>
     * It follows that for any two strings {@code s} and {@code t},
     * {@code s.intern() == t.intern()} is {@code true}
     * if and only if {@code s.equals(t)} is {@code true}.
     * <p>
     * All literal strings and string-valued constant expressions are
     * interned. String literals are defined in section 3.10.5 of the
     * <cite>The Java&trade; Language Specification</cite>.
     *
     * @return  a string that has the same contents as this string, but is
     *          guaranteed to be from a pool of unique strings.
     */
    public native String intern();
    /**
     * Returns a formatted string using the specified format string and
     * arguments.
     *
     * <p> The locale always used is the one returned by {@link
     * java.util.Locale#getDefault() Locale.getDefault()}.
     *
     * @param  format
     *         A <a href="../util/Formatter.html#syntax">format string</a>
     *
     * @param  args
     *         Arguments referenced by the format specifiers in the format
     *         string.  If there are more arguments than format specifiers, the
     *         extra arguments are ignored.  The number of arguments is
     *         variable and may be zero.  The maximum number of arguments is
     *         limited by the maximum dimension of a Java array as defined by
     *         <cite>The Java&trade; Virtual Machine Specification</cite>.
     *         The behaviour on a
     *         {@code null} argument depends on the <a
     *         href="../util/Formatter.html#syntax">conversion</a>.
     *
     * @throws  java.util.IllegalFormatException
     *          If a format string contains an illegal syntax, a format
     *          specifier that is incompatible with the given arguments,
     *          insufficient arguments given the format string, or other
     *          illegal conditions.  For specification of all possible
     *          formatting errors, see the <a
     *          href="../util/Formatter.html#detail">Details</a> section of the
     *          formatter class specification.
     *
     * @return  A formatted string
     *
     * @see  java.util.Formatter
     * @since  1.5
     */
    public static String format(String format, Object... args) {
        return new String(format + " format string not implemented yet");
    }
}
