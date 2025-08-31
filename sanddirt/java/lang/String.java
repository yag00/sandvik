package java.lang;
/**
 * @@sandvik modified
 * The {@code String} class represents character strings. All
 * string literals in Java programs, such as {@code "abc"}, are
 * implemented as instances of this class.
 */
public class String
    implements java.io.Serializable, java.lang.Comparable<String>, java.lang.CharSequence {
    /**
     * Create an empty string
     */
    public String() {}
    /**
     * Create a copy of a given string.
     * @param str the string to copy
     */
    public String(java.lang.String str) {}
    /**
     * Create a string from an array of byte.
     * @param val array of byte
     */
    public String(byte[] val) {}
    /**
     * Create a string from a part of array of byte.
     * @param val array of byte
     * @param offset index of the first byte
     * @param count number of byte to copy
     */
    public String(byte[] val, int offset, int count) {}
    /**
     * Create a string from an array of char.
     * @param val array of char
     */
    public String(char[] val) {}
    /**
     * Create a string from a part of array of char.
     * @param val array of char
     * @param offset index of the first char
     * @param count number of char to copy
     */
    public String(char[] val, int offset, int count) {}
    /**
     * Create a string from a part of array of codePoint.
     * @param val array of codePoint
     * @param offset index of the first codePoint
     * @param count number of codePoint to copy
     */
    public String(int[] val, int offset, int count) {}
    /**
     * Create a string from a part of array of byte.
     * @param val array of byte
     * @param hibyte not taken into account
     * @param offset index of the first byte
     * @param count number of byte to copy
     */
    public String(byte[] val, int hibyte, int offset, int count) {}
    /**
     * Create a string from a part of array of byte.
     * @param val array of byte
     * @param offset index of the first byte
     * @param count number of byte to copy
     * @param charSet not taken into account
     */
    public String(byte[] val, int offset, int count, java.lang.String charSet) {}
    /**
     * Create a copy of a given string.
     * @param str the string to copy
     */
    public String(java.lang.StringBuffer str) {}
    /**
     * Create a copy of a given string.
     * @param str the string to copy
     */
    public String(java.lang.StringBuilder str) {}
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
     * Return the first index of a given char in the String.
     * @param val char to search
     * @return index of the char in the String or -1 if not found
     */
    public native final int indexOf(int val);
    /**
     * Return the first index of a given char in the String beginning at a given index.
     * @param val char to search
     * @param pos index of first element
     * @return index of the char in the String or -1 if not found
     */
    public native final int indexOf(int val, int pos);
    /**
     * Return the first index of a given string in the String.
     * @param str string to search
     * @return index of the string in the String or -1 if not found
     */
    public native final int indexOf(java.lang.String str);
    /**
     * Return the first index of a given string in the String beginning at a given index.
     * @param str string to search
     * @param pos index of first element
     * @return index of the string in the String or -1 if not found
     */
    public native final int indexOf(java.lang.String str, int pos);
    /**
     * Return the last index of a given char in the String.
     * @param val char to search
     * @return index of the char in the String or -1 if not found
     */
    public native final int lastIndexOf(int val);
    /**
     * Return the last index of a given char in the String beginning at a given index.
     * @param val char to search
     * @param pos index of first element
     * @return index of the char in the String or -1 if not found
     */
    public native final int lastIndexOf(int val, int pos);
    /**
     * Return the last index of a given string in the String.
     * @param str string to search
     * @return index of the string in the String or -1 if not found
     */
    public native final int lastIndexOf(java.lang.String str);
    /**
     * Return the last index of a given string in the String beginning at a given index.
     * @param str string to search
     * @param pos index of first element
     * @return index of the string in the String or -1 if not found
     */
    public native final int lastIndexOf(java.lang.String str, int pos);
    /**
     * Return the length of the String.
     * @return length
     */
    public native final int length();
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
    public native final java.lang.String toUpperCase();
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
}
