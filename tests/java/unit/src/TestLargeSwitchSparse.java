public class TestLargeSwitchSparse {
    public static void main(String[] args) {
        boolean success = true;
        int val = 37;
        int out;
        switch(val) {
            case 0: out=0; break; case 3: out=3; break; case 7: out=7; break;
            case 13: out=13; break; case 20: out=20; break; case 37: out=37; break;
            case 50: out=50; break; case 99: out=99; break;
            default: out=-1; break;
        }

        if (out != 37) { System.out.println("fail: sparse switch"); success = false; }

        if (success) System.out.println("ok");
    }
}
