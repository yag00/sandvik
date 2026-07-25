public class TestJNIMethods extends BaseJNITest {

    static {
        System.loadLibrary("testjni");
    }

    // Call<X>Method tests
    public native Object nativeCallObjectMethod(CallTarget target, Object arg);
    public native Object nativeCallObjectMethodV(CallTarget target, Object arg);
    public native Object nativeCallObjectMethodA(CallTarget target, Object arg);

    public native boolean nativeCallBooleanMethod(CallTarget target, boolean v);
    public native boolean nativeCallBooleanMethodV(CallTarget target, boolean v);
    public native boolean nativeCallBooleanMethodA(CallTarget target, boolean v);

    public native byte nativeCallByteMethod(CallTarget target, byte v);
    public native byte nativeCallByteMethodV(CallTarget target, byte v);
    public native byte nativeCallByteMethodA(CallTarget target, byte v);

    public native char nativeCallCharMethod(CallTarget target, char v);
    public native char nativeCallCharMethodV(CallTarget target, char v);
    public native char nativeCallCharMethodA(CallTarget target, char v);

    public native short nativeCallShortMethod(CallTarget target, short v);
    public native short nativeCallShortMethodV(CallTarget target, short v);
    public native short nativeCallShortMethodA(CallTarget target, short v);

    public native int nativeCallIntMethod(CallTarget target, int v);
    public native int nativeCallIntMethodV(CallTarget target, int v);
    public native int nativeCallIntMethodA(CallTarget target, int v);

    public native long nativeCallLongMethod(CallTarget target, long v);
    public native long nativeCallLongMethodV(CallTarget target, long v);
    public native long nativeCallLongMethodA(CallTarget target, long v);

    public native float nativeCallFloatMethod(CallTarget target, float v);
    public native float nativeCallFloatMethodV(CallTarget target, float v);
    public native float nativeCallFloatMethodA(CallTarget target, float v);

    public native double nativeCallDoubleMethod(CallTarget target, double v);
    public native double nativeCallDoubleMethodV(CallTarget target, double v);
    public native double nativeCallDoubleMethodA(CallTarget target, double v);

    public native void nativeCallVoidMethod(CallTarget target, int marker);
    public native void nativeCallVoidMethodV(CallTarget target, int marker);
    public native void nativeCallVoidMethodA(CallTarget target, int marker);

    // CallNonvirtual<X>Method tests
    public native Object nativeCallNonvirtualObjectMethod(CallTarget target, Object arg);
    public native Object nativeCallNonvirtualObjectMethodV(CallTarget target, Object arg);
    public native Object nativeCallNonvirtualObjectMethodA(CallTarget target, Object arg);

    public native boolean nativeCallNonvirtualBooleanMethod(CallTarget target, boolean v);
    public native boolean nativeCallNonvirtualBooleanMethodV(CallTarget target, boolean v);
    public native boolean nativeCallNonvirtualBooleanMethodA(CallTarget target, boolean v);

    public native byte nativeCallNonvirtualByteMethod(CallTarget target, byte v);
    public native byte nativeCallNonvirtualByteMethodV(CallTarget target, byte v);
    public native byte nativeCallNonvirtualByteMethodA(CallTarget target, byte v);

    public native char nativeCallNonvirtualCharMethod(CallTarget target, char v);
    public native char nativeCallNonvirtualCharMethodV(CallTarget target, char v);
    public native char nativeCallNonvirtualCharMethodA(CallTarget target, char v);

    public native short nativeCallNonvirtualShortMethod(CallTarget target, short v);
    public native short nativeCallNonvirtualShortMethodV(CallTarget target, short v);
    public native short nativeCallNonvirtualShortMethodA(CallTarget target, short v);

    public native int nativeCallNonvirtualIntMethod(CallTarget target, int v);
    public native int nativeCallNonvirtualIntMethodV(CallTarget target, int v);
    public native int nativeCallNonvirtualIntMethodA(CallTarget target, int v);

    public native long nativeCallNonvirtualLongMethod(CallTarget target, long v);
    public native long nativeCallNonvirtualLongMethodV(CallTarget target, long v);
    public native long nativeCallNonvirtualLongMethodA(CallTarget target, long v);

    public native float nativeCallNonvirtualFloatMethod(CallTarget target, float v);
    public native float nativeCallNonvirtualFloatMethodV(CallTarget target, float v);
    public native float nativeCallNonvirtualFloatMethodA(CallTarget target, float v);

    public native double nativeCallNonvirtualDoubleMethod(CallTarget target, double v);
    public native double nativeCallNonvirtualDoubleMethodV(CallTarget target, double v);
    public native double nativeCallNonvirtualDoubleMethodA(CallTarget target, double v);

    public native void nativeCallNonvirtualVoidMethod(CallTarget target, int marker);
    public native void nativeCallNonvirtualVoidMethodV(CallTarget target, int marker);
    public native void nativeCallNonvirtualVoidMethodA(CallTarget target, int marker);

    public void testCallMethods() {
        CallTarget t = new CallTarget();
        String s = "callObj";

        assertEquals(s, nativeCallObjectMethod(t, s));
        assertEquals(s, nativeCallObjectMethodV(t, s));
        assertEquals(s, nativeCallObjectMethodA(t, s));

        assertEquals(true, nativeCallBooleanMethod(t, true));
        assertEquals(true, nativeCallBooleanMethodV(t, true));
        assertEquals(true, nativeCallBooleanMethodA(t, true));

        assertEquals((byte)5, nativeCallByteMethod(t, (byte)5));
        assertEquals((byte)5, nativeCallByteMethodV(t, (byte)5));
        assertEquals((byte)5, nativeCallByteMethodA(t, (byte)5));

        assertEquals('K', nativeCallCharMethod(t, 'K'));
        assertEquals('K', nativeCallCharMethodV(t, 'K'));
        assertEquals('K', nativeCallCharMethodA(t, 'K'));

        assertEquals((short)123, nativeCallShortMethod(t, (short)123));
        assertEquals((short)123, nativeCallShortMethodV(t, (short)123));
        assertEquals((short)123, nativeCallShortMethodA(t, (short)123));

        assertEquals(777, nativeCallIntMethod(t, 777));
        assertEquals(777, nativeCallIntMethodV(t, 777));
        assertEquals(777, nativeCallIntMethodA(t, 777));

        assertEquals(1234567890123L, nativeCallLongMethod(t, 1234567890123L));
        assertEquals(1234567890123L, nativeCallLongMethodV(t, 1234567890123L));
        assertEquals(1234567890123L, nativeCallLongMethodA(t, 1234567890123L));

        assertEquals(3.14f, nativeCallFloatMethod(t, 3.14f));
        assertEquals(3.14f, nativeCallFloatMethodV(t, 3.14f));
        assertEquals(3.14f, nativeCallFloatMethodA(t, 3.14f));

        assertEquals(6.28, nativeCallDoubleMethod(t, 6.28));
        assertEquals(6.28, nativeCallDoubleMethodV(t, 6.28));
        assertEquals(6.28, nativeCallDoubleMethodA(t, 6.28));

        nativeCallVoidMethod(t, 42);
        assertEquals(42, t.voidMarker);
        nativeCallVoidMethodV(t, 43);
        assertEquals(43, t.voidMarker);
        nativeCallVoidMethodA(t, 44);
        assertEquals(44, t.voidMarker);
    }

    public void testCallNonvirtualMethods() {
        CallTarget t = new CallTarget();
        String s = "callObjNV";

        assertEquals(s, nativeCallNonvirtualObjectMethod(t, s));
        assertEquals(s, nativeCallNonvirtualObjectMethodV(t, s));
        assertEquals(s, nativeCallNonvirtualObjectMethodA(t, s));

        assertEquals(true, nativeCallNonvirtualBooleanMethod(t, true));
        assertEquals(true, nativeCallNonvirtualBooleanMethodV(t, true));
        assertEquals(true, nativeCallNonvirtualBooleanMethodA(t, true));

        assertEquals((byte)5, nativeCallNonvirtualByteMethod(t, (byte)5));
        assertEquals((byte)5, nativeCallNonvirtualByteMethodV(t, (byte)5));
        assertEquals((byte)5, nativeCallNonvirtualByteMethodA(t, (byte)5));

        assertEquals('K', nativeCallNonvirtualCharMethod(t, 'K'));
        assertEquals('K', nativeCallNonvirtualCharMethodV(t, 'K'));
        assertEquals('K', nativeCallNonvirtualCharMethodA(t, 'K'));

        assertEquals((short)123, nativeCallNonvirtualShortMethod(t, (short)123));
        assertEquals((short)123, nativeCallNonvirtualShortMethodV(t, (short)123));
        assertEquals((short)123, nativeCallNonvirtualShortMethodA(t, (short)123));

        assertEquals(777, nativeCallNonvirtualIntMethod(t, 777));
        assertEquals(777, nativeCallNonvirtualIntMethodV(t, 777));
        assertEquals(777, nativeCallNonvirtualIntMethodA(t, 777));

        assertEquals(1234567890123L, nativeCallNonvirtualLongMethod(t, 1234567890123L));
        assertEquals(1234567890123L, nativeCallNonvirtualLongMethodV(t, 1234567890123L));
        assertEquals(1234567890123L, nativeCallNonvirtualLongMethodA(t, 1234567890123L));

        assertEquals(3.14f, nativeCallNonvirtualFloatMethod(t, 3.14f));
        assertEquals(3.14f, nativeCallNonvirtualFloatMethodV(t, 3.14f));
        assertEquals(3.14f, nativeCallNonvirtualFloatMethodA(t, 3.14f));

        assertEquals(3.14159, nativeCallNonvirtualDoubleMethod(t, 3.14159));
        assertEquals(3.14159, nativeCallNonvirtualDoubleMethodV(t, 3.14159));
        assertEquals(3.14159, nativeCallNonvirtualDoubleMethodA(t, 3.14159));

        nativeCallNonvirtualVoidMethod(t, 55);
        assertEquals(55, t.voidMarker);
        nativeCallNonvirtualVoidMethodV(t, 56);
        assertEquals(56, t.voidMarker);
        nativeCallNonvirtualVoidMethodA(t, 57);
        assertEquals(57, t.voidMarker);
    }

    public static void main(String[] args) {
        TestJNIMethods test = new TestJNIMethods();
        test.testCallMethods();
        test.testCallNonvirtualMethods();
        System.out.println(test.getSuccess() ? "ok" : "fail");
    }
}