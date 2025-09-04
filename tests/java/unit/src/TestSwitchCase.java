public class TestSwitchCase {
    public static void main(String[] args) {
        boolean success = true;

        // Nested if/else chains
        int x = 3;
        int y;
        if (x == 1) y = 10;
        else if (x == 2) y = 20;
        else if (x == 3) y = 30;
        else y = 0;
        if (y != 30) { System.out.println("fail: nested if"); success = false; }

        // While loop with continue/break
        int sum = 0;
        int i = 0;
        while (i < 5) {
            i++;
            if (i % 2 == 0) continue;
            if (i == 5) break;
            sum += i;
        }
        if (sum != 4) { System.out.println("fail: while loop jump"); success = false; }

        // Simulate switch with multiple cases
        int val = 2;
        int result;
        switch (val) {
            case 0: result = 10; break;
            case 1: result = 20; break;
            case 2: result = 30; break;
            default: result = -1; break;
        }
        if (result != 30) { System.out.println("fail: switch jump"); success = false; }

        if (success) System.out.println("ok");
    }
}

