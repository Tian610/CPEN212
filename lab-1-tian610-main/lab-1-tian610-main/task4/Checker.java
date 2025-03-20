public class Checker {

    public static void main(String[] args) {
        generatePalindromes();
    }

    public static void generatePalindromes() {
        char[] palindrome = new char[20];
        generatePalindromesHelper(palindrome, 0, 19);
    }
    
    private static void generatePalindromesHelper(char[] palindrome, int left, int right) {
        if (left > right) {
            calculateHash(new String(palindrome));
            return;
        }
    
        for (char c = 'A'; c <= 'Z'; c++) {
            palindrome[left] = c;
            palindrome[right] = c;
            generatePalindromesHelper(palindrome, left + 1, right - 1);
        }
    }

    public static void calculateHash(String password) {
        int w25 = 20;

        for (char ch : password.toCharArray()) {
            int w19 = ch; // Current character's ASCII value

            // Insert the LSB of w25 at bit position 8
            int lsb = w25 & 0x1; // Extract the LSB of w25
            w25 = (w25 & ~(0x1 << 7)) | (lsb << 7); // Clear bit 8 and set it to LSB
            
            // XOR with w19 and w25 shifted right by 1
            w25 = w19 ^ (w25 >>> 1); 
        }

        // Check if the hash is 228 as expected
        if (w25 == 228) {
            System.out.println(password);
        }
    }

}
