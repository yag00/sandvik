public class TestFilledNewArray {

    public static void main(String[] args) {
        boolean success = true;

        int[][] array = new int[][] { {1, 2, 3}, {4, 5} };
        
        if (array[0][0] != 1 || array[0][1] != 2 || array[0][2] != 3 || array[1][0] != 4 || array[1][1] != 5) {
            System.out.println("fail: int[][] { {1, 2, 3}, {4, 5} } => " + array);
            success = false;
        }

        if (success) System.out.println("ok");
    }
}

