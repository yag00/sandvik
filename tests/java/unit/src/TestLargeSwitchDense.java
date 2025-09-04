public class TestLargeSwitchDense {
    public static void main(String[] args) {
        boolean success = true;
        int val = 37;
        int out;
        switch(val) {
            case 0: out=0; break; case 1: out=1; break; case 2: out=2; break;
            case 3: out=3; break; case 4: out=4; break; case 5: out=5; break; case 6: out=6; break;
            case 7: out=7; break; case 8: out=8; break; case 9: out=9; break; case 10: out=10; break;
            case 11: out=11; break; case 12: out=12; break; case 13: out=13; break; case 14: out=14; break;
            case 15: out=15; break; case 16: out=16; break; case 17: out=17; break; case 18: out=18; break;
            case 19: out=19; break; case 20: out=20; break; case 21: out=21; break; case 22: out=22; break;
            case 23: out=23; break; case 24: out=24; break; case 25: out=25; break; case 26: out=26; break;
            case 27: out=27; break; case 28: out=28; break; case 29: out=29; break; case 30: out=30; break;
            case 31: out=31; break; case 32: out=32; break; case 33: out=33; break; case 34: out=34; break;
            case 35: out=35; break; case 36: out=36; break; case 37: out=37; break; case 38: out=38; break;
            case 39: out=39; break; case 40: out=40; break; case 41: out=41; break; case 42: out=42; break;
            case 43: out=43; break; case 44: out=44; break; case 45: out=45; break; case 46: out=46; break;
            case 47: out=47; break; case 48: out=48; break; case 49: out=49; break; default: out=-1; break;
        }

        if (out != 37) { System.out.println("fail: dense switch"); success = false; }

        if (success) System.out.println("ok");
    }
}

