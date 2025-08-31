package java.lang;
/**
 * @@sandvik modified
 * Class {@code Object} is the root of the class hierarchy.
 * Every class has {@code Object} as a superclass. All objects,
 * including arrays, implement the methods of this class.
 */
public class Object {
    /**
     * Return the class of the object.
     * @return object'class
     */
    public native java.lang.Class getClass();
    /**
     * Return a human readable representation of the object.
     * @return readable representation of the object
     */
    public native java.lang.String toString();
    /**
     * Indicates if the object is equal to the other.
     * @param other object to compare to
     * @return true if objects are equals
     */
    public native boolean equals(java.lang.Object other);
    /**
     * Return the hashCode of the object.
     * @return hashCode'object
     */
    public native int hashCode();
    /**
     * Clone the object.
     * @throws CloneNotSupportedException when object can not be cloned
     * @return a clone of the object
     */
    protected native java.lang.Object clone() throws CloneNotSupportedException;
}
