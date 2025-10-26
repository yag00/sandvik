public class TestThreadComplex {
	static class BankAccount {
		private int balance = 0;
		private final Object lock = new Object();

		public void deposit(int amount) {
			synchronized (lock) {
				int old = balance;
				balance = old + amount;
				System.err.println(Thread.currentThread().getName()
						+ " deposited " + amount + ", balance=" + balance);
				lock.notifyAll(); // wake up waiting threads
			}
		}

		public void withdraw(int amount) {
			synchronized (lock) {
				while (balance < amount) {
					try {
						System.err.println(Thread.currentThread().getName()
								+ " waiting for funds... balance=" + balance + ", need=" + amount);
						lock.wait();
					} catch (InterruptedException e) {
						System.out.println("fail: interrupted");
					}
				}
				balance -= amount;
				System.err.println(Thread.currentThread().getName()
						+ " withdrew " + amount + ", balance=" + balance);
			}
		}

		public int getBalance() {
			synchronized (lock) {
				return balance;
			}
		}
	}

	static class Depositor implements Runnable {
		private final BankAccount account;
		private final int amount;

		Depositor(BankAccount acc, int amount) {
			this.account = acc;
			this.amount = amount;
		}

		@Override
		public void run() {
			for (int i = 0; i < 5; i++) {
				account.deposit(amount);
				try {
					Thread.sleep(50);
				} catch (InterruptedException e) {
					System.out.println("fail: interrupted");
				}
			}
		}
	}

	static class Withdrawer implements Runnable {
		private final BankAccount account;
		private final int amount;

		Withdrawer(BankAccount acc, int amount) {
			this.account = acc;
			this.amount = amount;
		}

		@Override
		public void run() {
			for (int i = 0; i < 5; i++) {
				account.withdraw(amount);
				try {
					Thread.sleep(7);
				} catch (InterruptedException e) {
					System.out.println("fail: interrupted");
				}
			}
		}
	}

	public static void main(String[] args) {
		boolean success = true;

		BankAccount shared = new BankAccount();

		Thread depositor1 = new Thread(new Depositor(shared, 10), "Depositor-1");
		Thread depositor2 = new Thread(new Depositor(shared, 20), "Depositor-2");
		Thread withdrawer1 = new Thread(new Withdrawer(shared, 15), "Withdrawer-1");

		depositor1.start();
		depositor2.start();
		withdrawer1.start();

		try {
			depositor1.join();
			depositor2.join();
			withdrawer1.join();
		} catch (InterruptedException e) {
			System.out.println("fail: join interrupted");
			success = false;
		}

		int finalBalance = shared.getBalance();
		System.err.println("Final balance = " + finalBalance);

		// Each depositor: +5 * amount
		int expectedDeposit = (5 * 10) + (5 * 20); // 150 total deposits
		int expectedWithdraw = 5 * 15; // 75 total withdrawals
		int expectedFinal = expectedDeposit - expectedWithdraw; // 75

		if (finalBalance != expectedFinal) {
			System.out.println("fail: final balance mismatch (expected "
					+ expectedFinal + ", got " + finalBalance + ")");
			success = false;
		}

		Thread current = Thread.currentThread();
		if (current == null || !current.getName().equals("main")) {
			System.out.println("fail: currentThread name");
			success = false;
		}

		if (success) System.out.println("ok");
	}
}
