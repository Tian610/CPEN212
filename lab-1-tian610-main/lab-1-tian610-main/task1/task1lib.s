    // DO NOT MODIFY THIS FILE

    .text
    .global syscall
    // unsigned long syscall(unsigned long syscall_no, ...);
syscall:
    mov x8, x0 // syscall number
    mov	x0, x1 // remaining arguments
    mov	x1, x2 //         |
    mov	x2, x3 //         |
    mov	x3, x4 //         |
    mov	x4, x5 //         |
    mov	x5, x6 //         |
    mov	x6, x7 // --------+
    svc	0x0
    ret
