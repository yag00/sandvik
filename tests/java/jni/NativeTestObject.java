class NativeTestObject {
	public String message;
    public int intValue;
    public double doubleValue;
    public float floatValue;
    public long longValue;
    public short shortValue;
    public byte byteValue;
    public char charValue;
    public boolean booleanValue;

	public NativeTestObject(String msg, int i) {
		this.message = msg;
		this.intValue = i;
		this.doubleValue = 0;
		this.floatValue = 0;
		this.longValue = 0;
		this.shortValue = 0;
		this.byteValue = 0;
		this.charValue = 0;
		this.booleanValue = false;
	}
	public NativeTestObject(String msg, int i, double d, float f, long l, short s, byte b, char c, boolean bool) {
		this.message = msg;
		this.intValue = i;
		this.doubleValue = d;
		this.floatValue = f;
		this.longValue = l;
		this.shortValue = s;
		this.byteValue = b;
		this.charValue = c;
		this.booleanValue = bool;
	}
}