import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class TestRegex {

	static boolean success = true;

	public static void main(String[] args) {
		testSimpleMatch();
		testGroupExtract();
		testMultipleFind();
		if (success) {
			System.out.println("ok");
		}
	}

	static void testSimpleMatch() {
		String input = "abc123";
		Pattern p = Pattern.compile("abc\\d+");
		Matcher m = p.matcher(input);
		if (!m.matches()) {
			System.out.println("SimpleMatch: FAIL");
			success = false;
		}
	}

	static void testGroupExtract() {
		String input = "id=42";
		Pattern p = Pattern.compile("id=(\\d+)");
		Matcher m = p.matcher(input);
		if (m.find()) {
			String id = m.group(1);
			if (!id.equals("42")) {
				System.out.println("GroupExtract: FAIL (got " + id + ")");
				success = false;
			}
		} else {
			System.out.println("GroupExtract: FAIL (no match)");
			success = false;
		}
	}

	static void testMultipleFind() {
		String input = "x=7;y=12;z=99;";
		Pattern p = Pattern.compile("\\d+");
		Matcher m = p.matcher(input);
		int count = 0;
		while (m.find()) {
			count++;
			// System.out.println("Found: " + m.group());
		}
		if (count != 3) {
			System.out.println("MultipleFind: FAIL (found " + count + ")");
			success = false;
		}
	}
}
