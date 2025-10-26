public class TestThread2 {
    static class MyThread implements Runnable {
        private int counter = 0;
        private String message = "";

        @Override
        public void run() {
            for (int i = 0; i < 50; i++) {
                synchronized (this) { // protect shared instance
                    counter++;
                    message += "x";
                }
                try {
                    Thread.sleep(1);
                } catch (InterruptedException e) {
                    System.out.println("fail: interrupted");
                }
            }
        }

        public synchronized int getCounter() {
            return counter;
        }

        public synchronized String getMessage() {
            return message;
        }
    }

    public static void main(String[] args) {
        boolean success = true;

        // Shared instance between both threads
        MyThread shared = new MyThread();

        Thread t1 = new Thread(shared, "T1");
        Thread t2 = new Thread(shared, "T2");

        t1.start();
        t2.start();

        try {
            t1.join();
            t2.join();
        } catch (InterruptedException e) {
            System.out.println("fail: join interrupted");
            success = false;
        }

        // Each thread ran 50 times, shared counter should now be 100
        int result = shared.getCounter();
        String msg = shared.getMessage();

        if (result != 100) {
            System.out.println("fail: counter=" + result);
            success = false;
        }

        if (msg.length() != 100) {
            System.out.println("fail: message=" + msg);
            success = false;
        }

        if (success) System.out.println("ok");
    }
}
