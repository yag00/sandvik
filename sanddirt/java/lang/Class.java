package java.lang;
/**
 * @@sandvik modified
 * Instances of the class {@code Class} represent classes and
 * interfaces in a running Java application.  An enum is a kind of
 * class and an annotation is a kind of interface.  Every array also
 * belongs to a class that is reflected as a {@code Class} object
 * that is shared by all arrays with the same element type and number
 * of dimensions.  The primitive Java types ({@code boolean},
 * {@code byte}, {@code char}, {@code short},
 * {@code int}, {@code long}, {@code float}, and
 * {@code double}), and the keyword {@code void} are also
 * represented as {@code Class} objects.
 *
 * <p> {@code Class} has no public constructor. Instead {@code Class}
 * objects are constructed automatically by the Java Virtual Machine as classes
 * are loaded and by calls to the {@code defineClass} method in the class
 * loader.
 */
public class Class<T> implements java.io.Serializable {
    /**
     * Return {@code this} if the class is assignable as the subclass.
     * @param subclass class to assign
     * @param <U> subclass type
     * @return {@code this}
     * @throws ClassCastException if the class is not assignable to the subclass
     */
    public native <U> java.lang.Class<? extends U> asSubclass(java.lang.Class<U> subclass)
        throws ClassCastException;
    /**
     * Cast the specified object to the class.
     * @param object object to cast
     * @return the object
     * @throws ClassCastException if the object is not castable to the class
     */
    public native T cast(java.lang.Object object) throws ClassCastException;
    /**
     * Return the assertion status of the class.
     * @return always true in this implementation
     */
    public native boolean desiredAssertionStatus();
    /**
     * Indicates if the class is equal to another.
     * @param other class to compare to
     * @return true if classes are equals
     */
    public native boolean equals(java.lang.Object other);
    /**
     * Return the hashCode of the class.
     * @return hashCode value
     */
    public native int hashCode();
    /**
     * Return the class whose name is the given one.
     * @param className name of the class
     * @return object representing the class
     * @throws ClassNotFoundException when className does not exist
     */
    public native static java.lang.Class<?> forName(java.lang.String className)
        throws ClassNotFoundException;
    /**
     * Return a canonical representation string of the class.
     * @see #getName()
     * @return name of the class
     */
    public native java.lang.String getCanonicalName();
    /**
     * Return a class that represents the class of components of an array class.
     * @return the component class or null if not an array class
     */
    public native java.lang.Class<?> getComponentType();
    /**
     * Return all public innerClasses of the class.
     * @return array of public innerClasses
     */
    public native java.lang.Class<?>[] getClasses();
    /**
     * Return the public constructor method of the class.
     * @param paramsType array of parameters type of the desired constructor
     * @return constructor
     * @throws NoSuchMethodException if constructor is not public or does not exist
     */
    public native java.lang.reflect.Constructor<T> getConstructor(java.lang.Class<?>... paramsType)
        throws NoSuchMethodException;
    /**
     * Return an array of public constructors of the class.
     * @return array of public constructors
     */
    public native java.lang.reflect.Constructor<T>[] getConstructors();
    /**
     * Return all innerClasses of the class.
     * @return array of innerClasses
     */
    public native java.lang.Class<?>[] getDeclaredClasses();
    /**
     * Return the constructor method of the class.
     * @param paramsType array of parameters type of the desired constructor
     * @return constructor
     * @throws NoSuchMethodException if constructor does not exist
     */
    public native java.lang.reflect.Constructor<T> getDeclaredConstructor(
        java.lang.Class... paramsType) throws NoSuchMethodException;
    /**
     * Return an array of constructors of the class.
     * @return array of constructors
     */
    public native java.lang.reflect.Constructor<?>[] getDeclaredConstructors();
    /**
     * Return the field of the class whose name is the given one.
     * @param fieldName name of the field
     * @return field
     * @throws NoSuchFieldException if field does not exist
     */
    public native java.lang.reflect.Field getDeclaredField(java.lang.String fieldName)
        throws NoSuchFieldException;
    /**
     * Return all fields of the class.
     * @return array of fields
     */
    public native java.lang.reflect.Field[] getDeclaredFields();
    /**
     * Return the method of the class whose name and descriptor are the given one.
     * @param methodName name of the method
     * @param paramsType array of parameters type of the desired constructor
     * @return method
     * @throws NoSuchMethodException if method does not exist
     */
    public native java.lang.reflect.Method getDeclaredMethod(java.lang.String methodName,
                                                             java.lang.Class<?>... paramsType)
        throws NoSuchMethodException;
    /**
     * Return all methods of the class.
     * @return array of methods
     */
    public native java.lang.reflect.Method[] getDeclaredMethods();
    /**
     * Return the class that contains this class or null if not enclosed in any class.
     * @return class that contains this class (or null if this class is an anonymous innerClass)
     */
    public native java.lang.Class<?> getDeclaringClass();
    /**
     * Return the class that contains this class or null if not enclosed in any class.
     * @return class that contains this class
     */
    public native java.lang.Class<?> getEnclosingClass();
    /**
     * Return the constructor that contains this class or null if not enclosed in any constructor.
     * @return constructor that contains this class
     */
    public native java.lang.reflect.Constructor<?> getEnclosingConstructor();
    /**
     * Return the method that contains this class or null if not enclosed in any method.
     * @return method that contains this class
     */
    public native java.lang.reflect.Method getEnclosingMethod();
    /**
     * Return all enumerations of this class.
     * @return array of enumeration constants or null if not an enum class
     */
    public native T[] getEnumConstants();
    /**
     * Return the public field of the class whose name is the given one.
     * @param fieldName name of the field
     * @return field or null if not public
     * @throws NoSuchFieldException if field does not exist
     */
    public native java.lang.reflect.Field getField(java.lang.String fieldName)
        throws NoSuchFieldException;
    /**
     * Return all public fields of the class.
     * @return array of fields
     */
    public native java.lang.reflect.Field[] getFields();
    /**
     * Return all interfaces directly implemented by the class.
     * @return array of interfaces
     */
    public native java.lang.Class<?>[] getInterfaces();
    /**
     * Return the public method of the class whose name and descriptor are the given one.
     * @param methodName name of the method
     * @param paramsType array of parameters type of the desired constructor
     * @return method or null if not public
     * @throws NoSuchMethodException if method does not exist
     */
    public native java.lang.reflect.Method getMethod(java.lang.String methodName,
                                                     java.lang.Class<?>... paramsType)
        throws NoSuchMethodException;
    /**
     * Return all public methods of the class.
     * @return array of methods
     */
    public native java.lang.reflect.Method[] getMethods();
    /**
     * Return the modifiers of the class.
     * @return integer that represents a {@code logical or} of modifiers
     */
    public native int getModifiers();
    /**
     * Return the name of the class.
     * @return name of the class
     */
    public native java.lang.String getName();
    /**
     * Return the simple name of the class.
     * @return simple name of the class
     */
    public native java.lang.String getSimpleName();
    /**
     * Return the super class of the class.
     * @return the superclass
     */
    public native java.lang.Class<? super T> getSuperclass();
    /**
     * Indicates if the class is an annotation type.
     * @return true if annotation type
     */
    public native boolean isAnnotation();
    /**
     * Indicates if the class is an anonymous inner class.
     * @return true if anonymous inner class
     */
    public native boolean isAnonymousClass();
    /**
     * Indicates if the class is an array type class.
     * @return true if array type class
     */
    public native boolean isArray();
    /**
     * Indicates if the class is assignable to another class.
     * @param otherClass class to assign
     * @return true if assignable to the other class
     */
    public native boolean isAssignableFrom(java.lang.Class<?> otherClass);
    /**
     * Indicates if the class is a enum type class.
     * @return true if enum type class
     */
    public native boolean isEnum();
    /**
     * Indicates if an object is an instance of the class.
     * @param object object to check
     * @return true if instance of the class
     */
    public native boolean isInstance(java.lang.Object object);
    /**
     * Indicates if the class is a interface type class.
     * @return true if interface type class
     */
    public native boolean isInterface();
    /**
     * Indicates if the class is contained in a method.
     * @return true if contained in a method
     */
    public native boolean isLocalClass();
    /**
     * Indicates if the class is contained in a class.
     * @return true if contained in a class
     */
    public native boolean isMemberClass();
    /**
     * Indicates if the class is a primitive type class (such as {@code Boolean, Double, ...}).
     * @return true if primitive type class
     */
    public native boolean isPrimitive();
    /**
     * Return an object whose class is {@code this} class.
     * @return an instance of the class
     */
    public native T newInstance();
    /**
     * Return a representation string of the class including modifiers and parametrized types.
     * @return a representation string of the class
     */
    public native java.lang.String toGenericString();
    /**
     * Return a representation string of the class.
     * @return a representation string of the class
     */
    public native java.lang.String toString();
}
