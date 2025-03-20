// func3: Length function, store the length of the password in x25
0x1073158:  mov     x25, #0xffffffffffffffff    // Move -1 to x25
0x107315c:  ldrb    w9, [x3], #1                // Loads first digit of password. increments x3 by 1
0x1073160:  add     x25, x25, #0x1              // Adds 1 to x25 each loop
0x1073164:  cbnz    w9, 0x107315c               // Loops if a digit in password is not 0
0x1073168:  ret                                 // Returns

// func4
0x107316c:  mov     x25, #0x1                   // Move 1 to x25
// Loop:task2/code.s
    0x1073170:  ldrb    w27, [x3], #1           // Load 1 digit of x3 into w27 and increments x3 by 1
    0x1073174:  cbz     w27, 0x107319c          // Returns if digit is 0
    0x1073178:  ubfx    w26, w27, #6, #2        // Retrieves bit 6 and 7 of the character into w26
    0x107317c:  eor     w26, w26, #0x1          // XOR with 1, non-extended, str in w26
    0x1073180:  cbnz    w26, 0x1073198          // If w26 is non-zero, fails
    0x1073184:  and     w27, w27, #0x1f         // If zero, AND with 0x1F
    0x1073188:  subs    w27, w27, #0x1          // Computes w27 - 1 and sets flags
    0x107318c:  b.mi    0x1073198  // b.first   // If flag is negative, go to fails
    0x1073190:  subs    w27, w27, #0x1a         // Subtract 26 and set flag
    0x1073194:  b.mi    0x1073170  // b.first   // If flag is negative, check next digit
    0x1073198:  mov     x25, xzr                // If not, move 0 to x25
    0x107319c:  ret                             // Returns

// Program Start
0x10731a0:  ldr     x3, [sp]                    // x3 contains argument from sp
0x10731a4:  cmp     x3, #0x2                    // Check if x3=2
0x10731a8:  b.eq    0x10731cc                   // If not enough or too many arguments go to line 29, else go to 39

    // If more than one argument:           
    0x10731ac:  mov     x0, #0x2                // #2
    0x10731b0:  adr     x1, 0x107320e           // Set x1 to "One command argurment needed"
    0x10731b4:  mov     x2, #46                 // Sets x2 to 46
    0x10731b8:  mov     x8, #64                 // Sets x8 to 64 for write?
    0x10731bc:  svc     #0x0                    // Syscall to write to 
    0x10731c0:  mov     x0, #0x2                // Move 2 to x0
    0x10731c4:  mov     x8, #0x5d               // Move 93 to x8
    0x10731c8:  svc     #0x0                    // Exits Program

    // Else:
    0x10731cc:  ldr     x3, [sp, #16]           // Save the password in the sp
    0x10731d0:  bl      0x1073240               // Go to func1
    0x10731d4:  mov     x0, #0x1                // Move 1 to x0
    0x10731d8:  adr     x1, 0x1073207           // Set x1 to "FAILED"
    0x10731dc:  adr     x2, 0x1073200           // Set x2 to "PASSED"
    0x10731e0:  cmp     x25, #0x0               // Check if x25 is 0
    0x10731e4:  csel    x1, x1, x2, eq          // If equal, PASSED, if not, FAILED. Sets x1 to pass or fail
    0x10731e8:  mov     x2, #0x7                // Moves 7 to x2
    0x10731ec:  mov     x8, #0x40               // Moves 64 to x8
    0x10731f0:  svc     #0x0                    // Syscall for write
    0x10731f4:  mov     x0, #0x0                // Moves 0 to x0
    0x10731f8:  mov     x8, #0x5d               // Move 93 to x8
    0x10731fc:  svc     #0x0                    // Exits Program

// If x25 is 0, we pass. A lot of things mess with x25 so x25 is the true false flag for the password

// Check 1: Is the password a palindrome?
// Check 2: The password should be 20 digits long
// Check 3: Some sort of Hashing?
// Check 4: All digits must be uppercase alphabetical

// func1: Should be where the main checks lie

// Check 1
0x1073240:  stp     x30, x3, [sp, #-16]!        // Store x30 and x3 to stack in pos 16 and 8 respectively
0x1073244:  bl      0x1073290                   // Go to func2
0x1073248:  cbz     x25, 0x1073284              // If x25 = 0, go to final

// Check 2
0x107324c:  ldr     x3, [sp, #8]                // Load stack8 into x3
0x1073250:  bl      0x1073158                   // Go to func3
0x1073254:  cmp     x25, #0x14                  // Check if x25 = 20
0x1073258:  b.ne    0x1073284  // b.any         // If x25 != 14, go to final

// Check 3
0x107325c:  ldr     x3, [sp, #8]                // Load stack8 into x3
0x1073260:  bl      0x10732c4                   // Go to func5
0x1073264:  cmp     x25, #0xe4                  // Checks if x25 = 228
0x1073268:  b.ne    0x1073284  // b.any         // If x25 != 228, go to final

// Check 4
0x107326c:  ldr     x3, [sp, #8]                // Load stack8 into x3
0x1073270:  bl      0x107316c                   // Go to func4
0x1073274:  cbz     x25, 0x1073284              // If x25 = 0, go to final

// If all checks successful, x25 = 1
0x1073278:  mov     x25, #0x1                   // Set x25 to 1

0x107327c:  ldr     x30, [sp], #16              // Load x30 from the sp
0x1073280:  ret                                 // Returns
// Final
    0x1073284:  mov     x25, #0x0               // Set x25 to 0
    0x1073288:  ldr     x30, [sp], #16          // Load x30 from the sp
    0x107328c:  ret                             // Returns

// func2: Checks if the password is a palindrome!
0x1073290:  stp     x30, x3, [sp, #-16]!        // Store x30 and x3 to stack in pos 16 and 8 respectively
0x1073294:  bl      0x1073158                   // Go to func3. Figure out how long the password is
0x1073298:  ldp     x30, x3, [sp], #16          // Restores Password and LR
0x107329c:  add     x12, x3, x25                // store the sum of x3 and x25 in x12
0x10732a0:  mov     x25, #0x1                   // Set x25 to 1
    // Loop
    0x10732a4:  cmp     x12, x3                 // Check if x12 and x3 are equal
    0x10732a8:  b.ls    0x10732c0  // b.plast   // If x12 <= x3, Return
    0x10732ac:  ldrb    w23, [x3], #1           // Load a digit of password to w23 and increment x3 for next digit
    0x10732b0:  ldrb    w20, [x12, #-1]!        // Load a digit of x12 into w20 and decrement x12 for next digit
    0x10732b4:  cmp     w23, w20                // Compare the two digits
    0x10732b8:  b.eq    0x10732a4  // b.none    // If they're equal, go to loop
    0x10732bc:  mov     x25, #0x0               // Set x25 to 0
    0x10732c0:  ret                             // Returns

// func5: Changes w25 for every single character of the password. Some sort of checksum
0x10732c4:  stp     x30, x3, [sp, #-16]!        // Store x30 and x3 to stack in pos 16 and 8 respectively
0x10732c8:  bl      0x1073158                   // Get length of password in x25
0x10732cc:  ldp     x30, x3, [sp], #16          // Restore x30 and x3
    // Loop
    0x10732d0:  ldrb    w19, [x3], #1           // Load a digit of password to w19 and increment x3 for next digit
    0x10732d4:  bfi     w25, w25, #8, #1        // Inserts LSB of w25 into w25 at position 8
    0x10732d8:  eor     w25, w19, w25, lsr #1   // Shifts w25 1 to the right, XORs with W19, and stores in w25
    0x10732dc:  cbnz    w19, 0x10732d0          // If w19 != 0, loop
    0x10732e0:  ret                             // Returns


x3 = Callee Saved
x8 = Syscall parameter
x9 = Caller Saved
x30 = likely link register


