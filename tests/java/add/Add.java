public class Add {
	public static int add(int a, int b) {
		return a + b;
	}
	public static void main(String[] args) {
		if (args.length < 2) {
			System.out.println("Please provide two integers as arguments.");
			return;
		}
		try {
			int num1 = Integer.parseInt(args[0]);
			int num2 = Integer.parseInt(args[1]);
			System.out.println("Result: " + add(num1, num2));
		} catch (NumberFormatException e) {
			System.out.println("Invalid input. Please provide valid integers.");
		}
	}
}