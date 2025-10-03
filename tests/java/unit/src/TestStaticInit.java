class StaticA {
    static int val = 10;
    static {
        val = 42;
    }
}

class StaticB {
    static final int CONST = 100;
    static int init = compute();

    static int compute() { return StaticA.val + CONST; }
}

public class TestStaticInit {
    public static void main(String[] args) {
        boolean success = true;

        if (StaticA.val != 42) { System.out.println("fail: StaticA init " + StaticA.val); success = false; }
        if (StaticB.init != 142) { System.out.println("fail: StaticB init " + StaticB.init); success = false; }

        if (success) System.out.println("ok");
    }
}
