interface MyCollection {
    void add(int x);
    int get(int i);
    int size();
}

class MyArrayList implements MyCollection {
    private int[] arr = new int[10];
    private int count = 0;

    public void add(int x) {
        if (count >= arr.length) {
            int[] bigger = new int[arr.length * 2];
            for (int i = 0; i < arr.length; i++) bigger[i] = arr[i];
            arr = bigger;
        }
        arr[count++] = x;
    }

    public int get(int i) { return arr[i]; }
    public int size() { return count; }
}

public class TestCollectionsLike {
    public static void main(String[] args) {
        boolean success = true;

        MyCollection col = new MyArrayList();
        for (int i = 0; i < 20; i++) col.add(i);

        if (col.size() != 20) { System.out.println("fail: size"); success = false; }
        if (col.get(5) != 5) { System.out.println("fail: get(5)"); success = false; }

        int sum = 0;
        for (int i = 0; i < col.size(); i++) sum += col.get(i);
        if (sum != 190) { System.out.println("fail: sum"); success = false; }

        if (success) System.out.println("ok");
    }
}
