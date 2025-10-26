public class TestThread {
    static class MyThread implements Runnable {
        public static int counter = 0;

        @Override
        public void run() {
            for (int i = 0; i < 50; i++) {
                incrementCounter();
                try {
                    Thread.sleep(1);
                } catch (InterruptedException e) {
                    System.out.println("fail: interrupted");
                }
            }
        }

        private static synchronized void incrementCounter() {
            counter++;
        }
    }

    public static void main(String[] args) {
        boolean success = true;

        MyThread r1 = new MyThread();
        MyThread r2 = new MyThread();
        Thread t1 = new Thread(r1);
        Thread t2 = new Thread(r2);

        t1.start();
        t2.start();

        try {
            t1.join();
            t2.join();
        } catch (InterruptedException e) {
            System.out.println("fail: join interrupted");
            success = false;
        }

        // Each thread increments counter 50 times
        if (MyThread.counter != 100) {
            System.out.println("fail: counter=" + MyThread.counter);
            success = false;
        }

        // Test Thread.currentThread()
        Thread current = Thread.currentThread();
        if (current == null || !current.getName().equals("main")) {
            System.out.println("fail: currentThread name");
            success = false;
        }

        if (success) System.out.println("ok");
    }
}
