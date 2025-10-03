public class TestSwitchOnString {
    public static void main(String[] args) {
        boolean success = true;

        String key = "foo";
        int result;
        switch (key) {
            case "bar": result = 1; break;
            case "foo": result = 2; break;
            default: result = -1; break;
        }

        if (result != 2) { System.out.println("fail: switch string"); success = false; }

        if (success) System.out.println("ok");
    }
}