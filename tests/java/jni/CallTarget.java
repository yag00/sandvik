public class CallTarget {
    public int voidMarker = 0;
    
    public Object identityObject(Object o) { return o; }
    public boolean identityBoolean(boolean b) { return b; }
    public byte identityByte(byte b) { return b; }
    public char identityChar(char c) { return c; }
    public short identityShort(short s) { return s; }
    public int identityInt(int i) { return i; }
    public long identityLong(long l) { return l; }
    public float identityFloat(float f) { return f; }
    public double identityDouble(double d) { return d; }
    public void identityVoid(int marker) { this.voidMarker = marker; }
    
    public static int staticVoidMarker = 0;

    public static Object staticIdentityObject(Object o) { return o; }
    public static boolean staticIdentityBoolean(boolean b) { return b; }
    public static byte staticIdentityByte(byte b) { return b; }
    public static char staticIdentityChar(char c) { return c; }
    public static short staticIdentityShort(short s) { return s; }
    public static int staticIdentityInt(int i) { return i; }
    public static long staticIdentityLong(long l) { return l; }
    public static float staticIdentityFloat(float f) { return f; }
    public static double staticIdentityDouble(double d) { return d; }
    public static void staticIdentityVoid(int marker) { staticVoidMarker = marker; }
}
