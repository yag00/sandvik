public class TestDeepLoops {
    public static void main(String[] args) {
        boolean success = true;

        int result = 0;
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 4; j++) {
                for (int k = 0; k < 3; k++) {
                    result += i + j + k;
                }
            }
        }
        if (result != 270) {
            System.out.println("fail: deep loops sum " + result);
            success = false;
        }

        if (success) System.out.println("ok");
    }
}
