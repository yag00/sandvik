public class User {
 
        protected boolean authenticated = false;
 
        public void setAuthenticatedTrue() {
                this.authenticated = true;
        }
 
        public static void main(String[] args) {
                User user = new User();
                user.run();
        }
 
        public void run() {
                if (this.authenticated == false) {
                        System.out.println("Access Revoked!");
                }
                else {
                        System.out.println("Access Granted!");
                }
        }
}
