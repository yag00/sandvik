enum Color { RED, GREEN, BLUE }

public class TestEnums {
    public static void main(String[] args) {
        boolean success = true;

        Color c = Color.GREEN;
        if (c.ordinal() != 1) { System.out.println("fail: ordinal"); success = false; }

        Color[] vals = Color.values();
        if (vals.length != 3) { System.out.println("fail: values length"); success = false; }

        int result = -1;
        switch (c) {
            case RED: result = 0; break;
            case GREEN: result = 1; break;
            case BLUE: result = 2; break;
        }
        if (result != 1) { System.out.println("fail: enum switch"); success = false; }

        if (success) System.out.println("ok");
    }
}