package java.lang.reflect;
/**
 * @@sandvik modified
 * A {@code Field} provides information about, and dynamic access to, a
 * single field of a class or an interface.  The reflected field may
 * be a class (static) field or an instance field.
 *
 * <p>A {@code Field} permits widening conversions to occur during a get or
 * set access operation, but throws an {@code IllegalArgumentException} if a
 * narrowing conversion would occur.
 *
 * @see Member
 * @see java.lang.Class
 * @see java.lang.Class#getFields()
 * @see java.lang.Class#getField(String)
 * @see java.lang.Class#getDeclaredFields()
 * @see java.lang.Class#getDeclaredField(String)
 *
 * @author Kenneth Russell
 * @author Nakul Saraiya
 */
public class Field extends java.lang.reflect.AccessibleObject {
    private String name;
    /**
     * Indicates if the field is equal to another.
     * @param other field to compare to
     * @return true if fields are equals
     */
    public native boolean equals(java.lang.Object other);
    /**
     * Return the value of the field of a given object.
     * @param object instance of the object
     * @return value of the field
     * @throws IllegalArgumentException if object is not an instance of the class of the field
     */
    public native java.lang.Object get(java.lang.Object object) throws IllegalArgumentException;
    /**
     * Return the value of the field of a given object.
     * @param object instance of the object
     * @return value of the field
     * @throws IllegalArgumentException if object is not an instance of the class of the field or
     *     field is not a boolean type
     */
    public native boolean getBoolean(java.lang.Object object) throws IllegalArgumentException;
    /**
     * Return the value of the field of a given object.
     * @param object instance of the object
     * @return value of the field
     * @throws IllegalArgumentException if object is not an instance of the class of the field or
     *     field is not a byte type
     */
    public native byte getByte(java.lang.Object object) throws IllegalArgumentException;
    /**
     * Return the value of the field of a given object.
     * @param object instance of the object
     * @return value of the field
     * @throws IllegalArgumentException if object is not an instance of the class of the field or
     *     field is not a char type
     */
    public native char getChar(java.lang.Object object) throws IllegalArgumentException;
    /**
     * Return the value of the field of a given object.
     * @param object instance of the object
     * @return value of the field
     * @throws IllegalArgumentException if object is not an instance of the class of the field or
     *     field is not a double type
     */
    public native double getDouble(java.lang.Object object) throws IllegalArgumentException;
    /**
     * Return the value of the field of a given object.
     * @param object instance of the object
     * @return value of the field
     * @throws IllegalArgumentException if object is not an instance of the class of the field or
     *     field is not a float type
     */
    public native float getFloat(java.lang.Object object) throws IllegalArgumentException;
    /**
     * Return the value of the field of a given object.
     * @param object instance of the object
     * @return value of the field
     * @throws IllegalArgumentException if object is not an instance of the class of the field or
     *     field is not a integer type
     */
    public native int getInt(java.lang.Object object) throws IllegalArgumentException;
    /**
     * Return the value of the field of a given object.
     * @param object instance of the object
     * @return value of the field
     * @throws IllegalArgumentException if object is not an instance of the class of the field or
     *     field is not a long type
     */
    public native long getLong(java.lang.Object object) throws IllegalArgumentException;
    /**
     * Return the value of the field of a given object.
     * @param object instance of the object
     * @return value of the field
     * @throws IllegalArgumentException if object is not an instance of the class of the field or
     *     field is not a short type
     */
    public native short getShort(java.lang.Object object) throws IllegalArgumentException;
    /**
     * Return the class of the field.
     * @return class of the field
     */
    public native java.lang.Class getDeclaringClass();
    /**
     * Return the modifiers of the field.
     * @return integer that represents a {@code logical or} of modifiers
     */
    public native int getModifiers();
    /**
     * Return the name of the field.
     * @return name of the field
     */
    public native java.lang.String getName();
    /**
     * Return the class representing the type of the field.
     * @return type of the field
     */
    public native java.lang.Class getType();
    /**
     * Return the hashCode of the field.
     * @return hashCode value
     */
    public native int hashCode();
    /**
     * Indicates if the field is an enum type.
     * @return true if enum type
     */
    public native boolean isEnumConstant();
    /**
     * Indicates if the field is generated by the compiler.
     * @return true if generated by the compiler
     */
    public native boolean isSynthetic();
    /**
     * Set the value of the field of the given object.
     * @param object the given object
     * @param value value of the field
     * @throws IllegalArgumentException if the object is not an instance of the class of the field
     */
    public native void set(java.lang.Object object, java.lang.Object value)
        throws IllegalArgumentException;
    /**
     * Set the value of the field of the given object.
     * @param object the given object
     * @param value value of the field
     * @throws IllegalArgumentException if the object is not an instance of the class of the field
     *     or field is not a boolean type
     */
    public native void setBoolean(java.lang.Object object, boolean value)
        throws IllegalArgumentException;
    /**
     * Set the value of the field of the given object.
     * @param object the given object
     * @param value value of the field
     * @throws IllegalArgumentException if the object is not an instance of the class of the field
     *     or field is not a byte type
     */
    public native void setByte(java.lang.Object object, byte value) throws IllegalArgumentException;
    /**
     * Set the value of the field of the given object.
     * @param object the given object
     * @param value value of the field
     * @throws IllegalArgumentException if the object is not an instance of the class of the field
     *     or field is not a char type
     */
    public native void setChar(java.lang.Object object, char value) throws IllegalArgumentException;
    /**
     * Set the value of the field of the given object.
     * @param object the given object
     * @param value value of the field
     * @throws IllegalArgumentException if the object is not an instance of the class of the field
     *     or field is not a double type
     */
    public native void setDouble(java.lang.Object object, double value)
        throws IllegalArgumentException;
    /**
     * Set the value of the field of the given object.
     * @param object the given object
     * @param value value of the field
     * @throws IllegalArgumentException if the object is not an instance of the class of the field
     *     or field is not a float type
     */
    public native void setFloat(java.lang.Object object, float value)
        throws IllegalArgumentException;
    /**
     * Set the value of the field of the given object.
     * @param object the given object
     * @param value value of the field
     * @throws IllegalArgumentException if the object is not an instance of the class of the field
     *     or field is not a integer type
     */
    public native void setInt(java.lang.Object object, int value) throws IllegalArgumentException;
    /**
     * Set the value of the field of the given object.
     * @param object the given object
     * @param value value of the field
     * @throws IllegalArgumentException if the object is not an instance of the class of the field
     *     or field is not a long type
     */
    public native void setLong(java.lang.Object object, long value) throws IllegalArgumentException;
    /**
     * Set the value of the field of the given object.
     * @param object the given object
     * @param value value of the field
     * @throws IllegalArgumentException if the object is not an instance of the class of the field
     *     or field is not a short type
     */
    public native void setShort(java.lang.Object object, short value)
        throws IllegalArgumentException;
    /**
     * Return a representation string of the field.
     * @return a representation string of the field
     */
    public native java.lang.String toString();
}
