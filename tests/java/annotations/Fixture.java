import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.util.List;

public class Fixture {
	@Retention(RetentionPolicy.RUNTIME)
	@Target({ElementType.METHOD, ElementType.PARAMETER})
	@interface Meta {
		int intVal();

		String strVal();

		boolean boolVal();

		String[] arrVal();
	}

	static class Inner {
		int value() {
			return 42;
		}
	}

	List<String> genericField;

	@Meta(intVal = 7, strVal = "hi", boolVal = true, arrVal = {"a", "b"})
	void annotated(@Meta(intVal = 1, strVal = "p", boolVal = false, arrVal = {}) int x) {
	}

	public static void main(String[] args) {
		Inner i = new Inner();
		System.out.println(i.value());
	}
}
