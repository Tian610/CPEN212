import commands

def generate_palindromes():
    palindrome = [''] * 20
    generate_palindromes_helper(palindrome, 0, len(palindrome)-1)

def generate_palindromes_helper(palindrome, left, right):
    if left > right:
        palindrome_str = ''.join(palindrome)
        s = commands.getoutput("../../task2 " + palindrome_str)
        if s == "PASSED":
            print(palindrome_str)
        return

    for c in (chr(i) for i in range(ord('A'), ord('Z') + 1)):
        palindrome[left] = c
        palindrome[right] = c
        generate_palindromes_helper(palindrome, left + 1, right - 1)

if __name__ == "__main__":
    generate_palindromes()
