#ifndef __TASK1_H__
#define __TASK1_H__

// DO NOT MODIFY THIS FILE

// Q. What is the address at which the program starts executing (i.e., the address which corresponds to _start)?

// Returns: the start address of the program
void *program_start_addr();


// Q. Identify all functions in task1. We will define a function as a target of a bl (or blr) instruction. For each function, identify the full range of addresses this function resides at (first address and last address), as well as what this function does. Hint: most of these were directly taken from lecture.

// Returns: the address of the first instruction of the first function in the program
void *function_1_start_instr_addr();

// Returns: the address of the last instruction of the first function in the program
void *function_1_last_instr_addr();

// Returns: the address of the first instruction of the second function in the program
void *function_2_start_instr_addr();

// Returns: the address of the last instruction of the second function in the program
void *function_2_last_instr_addr();


// Q. task1 uses a single command-line argument. Identify which instructions are involved in checking that there are two argument and retrieving one argument.

// Returns: the address of the instruction that puts the number of command-line arguments in a register
void *arg_count_instr_addr();

// Returns: the address of the instruction that puts the address of the first command-line argument in a register
void *first_arg_instr_addr();

#endif // __TASK1_H__
