import java.util.concurrent.atomic.AtomicInteger;

public class TestAtomicInteger {
    public static void main(String[] args) {
        boolean success = true;

        AtomicInteger atomic = new AtomicInteger(0);

        // Basic set/get
        atomic.set(42);
        if (atomic.get() != 42) {
            System.out.println("fail: set/get");
            success = false;
        }

        // getAndIncrement / incrementAndGet
        int prev = atomic.getAndIncrement(); // should return 42, value becomes 43
        if (prev != 42 || atomic.get() != 43) {
            System.out.println("fail: getAndIncrement");
            success = false;
        }

        int after = atomic.incrementAndGet(); // value becomes 44, returns 44
        if (after != 44 || atomic.get() != 44) {
            System.out.println("fail: incrementAndGet");
            success = false;
        }

        // compareAndSet
        boolean casSuccess = atomic.compareAndSet(44, 100);
        if (!casSuccess || atomic.get() != 100) {
            System.out.println("fail: compareAndSet success");
            success = false;
        }

        boolean casFail = atomic.compareAndSet(44, 200);
        if (casFail || atomic.get() != 100) {
            System.out.println("fail: compareAndSet fail");
            success = false;
        }

        // Multi-threaded increments
        atomic.set(0);
        Thread t1 = new Thread(() -> {
            for (int i = 0; i < 500; i++) atomic.incrementAndGet();
        });
        Thread t2 = new Thread(() -> {
            for (int i = 0; i < 500; i++) atomic.incrementAndGet();
        });

        t1.start();
        t2.start();

        try {
            t1.join();
            t2.join();
        } catch (InterruptedException e) {
            System.out.println("fail: join interrupted");
            success = false;
        }

        if (atomic.get() != 1000) {
            System.out.println("fail: multi-threaded increments = " + atomic.get());
            success = false;
        }

        if (success) System.out.println("ok");
    }
}
