public class TestLoops {
    public static void main(String[] args) {
        int sum = 0;
        for (int i = 0; i < 5; i++) sum += i;

        int fact = 1, n = 5;
        while (n > 1) fact *= n--;

        if (sum == 10 && fact == 120) {
            System.out.println("ok");
        } else {
            System.out.println("fail");
        }
    }
}

