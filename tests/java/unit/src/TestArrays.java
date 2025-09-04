public class TestArrays {
    public static void main(String[] args) {
        boolean success = true;
        int[] arr = new int[3];
        arr[0] = 42;

        if (arr.length != 3) { System.out.println("fail: length"); success = false; }
        if (arr[0] != 42) { System.out.println("fail: value"); success = false; }

        try {
            int x = arr[5];
            System.out.println("fail: no exception");
            success = false;
        } catch (ArrayIndexOutOfBoundsException e) {
            // expected
        }

        if (success) System.out.println("ok");
    }
}
