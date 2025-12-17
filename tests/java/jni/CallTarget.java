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
}
