public class TestInterfaceArray {
    interface Marker {}

    static class Impl implements Marker {}

    public static void main(String[] args) {
        Marker[] arr = new Marker[2];
        arr[0] = new Impl();

        if (arr[0] instanceof Marker) {
            System.out.println("ok");
        } else {
            System.out.println("fail: array");
        }
    }
}
