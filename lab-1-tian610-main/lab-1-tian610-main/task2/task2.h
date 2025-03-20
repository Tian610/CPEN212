#ifndef __TASK2_H__
#define __TASK2_H__

// DO NOT MODIFY THIS FILE

// Returns: the address of the first instruction in the first check (i.e., the check at the lowest address in memory)
void *check_1_start_instr_addr();

// Returns: the address of the first instruction in the second check
void *check_2_start_instr_addr();

// Returns: the address of the first instruction in the third check
void *check_3_start_instr_addr();

// Returns: the address of the first instruction in the fourth check
void *check_4_start_instr_addr();


// You will use a subset of the flags below to report how each register is used.
// We are only interested in the convention for calling functions, not system calls.
// Note that you can report multiple flags by combining them with | (bitwise or):
// for example, if a register could be used to pass an argument and to return a value
// you might report "ARGUMENT | RETURN_VALUE | anyotherflagshere".
// Uses of the lower 32 bits of any register should be reported as if the full 64-bit
// register were used: e.g., if values are returned in w7, report that x7 holds return values.
// If the code in the executable you are analyzing is not sufficient to determine
// anything about how a register should be used (e.g., because it's never used),
// report UNKNOWN.

typedef enum {
    UNKNOWN = 0,       // nothing can be determined about this register from the task2 executable
    ARGUMENT = 1,      // register is used to pass an argument to a function
    RETURN_VALUE = 2,  // register is used to pass the return value of a function back to the caller
    LINK_REGISTER = 4, // register is used to pass the return address to a function call
    CALLER_SAVED = 8,  // register can be modified by the function and must be saved by the caller
    CALLEE_SAVED = 16, // register must be preserved across the function call
} call_conv_info_t;

// calling convention information about x0
call_conv_info_t x0_info();

// calling convention information about x1
call_conv_info_t x1_info();

// calling convention information about x2
call_conv_info_t x2_info();

// calling convention information about x3
call_conv_info_t x3_info();

// calling convention information about x4
call_conv_info_t x4_info();

// calling convention information about x5
call_conv_info_t x5_info();

// calling convention information about x6
call_conv_info_t x6_info();

// calling convention information about x7
call_conv_info_t x7_info();

// calling convention information about x8
call_conv_info_t x8_info();

// calling convention information about x9
call_conv_info_t x9_info();

// calling convention information about x10
call_conv_info_t x10_info();

// calling convention information about x11
call_conv_info_t x11_info();

// calling convention information about x12
call_conv_info_t x12_info();

// calling convention information about x13
call_conv_info_t x13_info();

// calling convention information about x14
call_conv_info_t x14_info();

// calling convention information about x15
call_conv_info_t x15_info();

// calling convention information about x16
call_conv_info_t x16_info();

// calling convention information about x17
call_conv_info_t x17_info();

// calling convention information about x18
call_conv_info_t x18_info();

// calling convention information about x19
call_conv_info_t x19_info();

// calling convention information about x20
call_conv_info_t x20_info();

// calling convention information about x21
call_conv_info_t x21_info();

// calling convention information about x22
call_conv_info_t x22_info();

// calling convention information about x23
call_conv_info_t x23_info();

// calling convention information about x24
call_conv_info_t x24_info();

// calling convention information about x25
call_conv_info_t x25_info();

// calling convention information about x26
call_conv_info_t x26_info();

// calling convention information about x27
call_conv_info_t x27_info();

// calling convention information about x28
call_conv_info_t x28_info();

// calling convention information about x29
call_conv_info_t x29_info();

// calling convention information about x30
call_conv_info_t x30_info();

#endif  // __TASK2_H__
