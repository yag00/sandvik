public class TestMonitorReentrant {

    static class ReentrantTest {
        static int counter = 0;

        static synchronized void level1() {
            level2();
        }

        static synchronized void level2() {
            level3();
        }

        static synchronized void level3() {
            counter++;
        }
    }

    public static void main(String[] args) {
        boolean success = true;

        ReentrantTest.level1();

        if (ReentrantTest.counter != 1) {
            System.out.println("fail: reentrant counter=" + ReentrantTest.counter);
            success = false;
        }

        if (success) System.out.println("ok");
    }
}
