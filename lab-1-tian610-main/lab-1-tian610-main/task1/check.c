#include "task1lib.h"
#include "task1.h"

// print zero-terminated string
static void print(const char *s) {
    while (*s) syscall(64, 1, s++, 1);
}

// print address as hexadecimal
static void print_ptr(const void *p) {
    unsigned long n = (unsigned long) p;
    const char *ds = "0123456789abcdef";
    char buf[17];
    char *b = buf + 16;
    *b = '\0';
    while (b != buf) { *--b = ds[n & 15]; n >>= 4; }
    print(buf);
}

static void exit(unsigned long exitcode) {
    syscall(93, exitcode);
}

void _start() {
    print("program start:    ");
    print_ptr(program_start_addr());
    print("\nfn 1 start instr: ");
    print_ptr(function_1_start_instr_addr());
    print("\nfn 1 last instr:  ");
    print_ptr(function_1_last_instr_addr());
    print("\nfn 2 start instr: ");
    print_ptr(function_2_start_instr_addr());
    print("\nfn 2 last instr:  ");
    print_ptr(function_2_last_instr_addr());
    print("\narg count instr:  ");
    print_ptr(arg_count_instr_addr());
    print("\nfirst arg instr:  ");
    print_ptr(first_arg_instr_addr());
    print("\n");
    exit(0);
}
