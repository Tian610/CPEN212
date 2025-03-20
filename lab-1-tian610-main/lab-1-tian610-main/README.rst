************************************
Lab 1: Rapid Unplanned (Dis)assembly
************************************

.. contents:: Contents
   :depth: 2


TL;DR
=====

Deadlines
---------

- Tasks 1 and 2: January 17th, 23:59 Vancouver time.

- Tasks 3 and 4: January 21st, 23:59 Vancouver time.

- Bonus (optional): January 21st, 23:59 Vancouver time.


Logistics
---------

You will need to use the ``castor.ece.ubc.ca`` server to complete this lab; you can access it using your ECE account credentials. We will test all of your answers by compliing and running the code you submit on that machine, so if your submission does not compile or run there (e.g., because you didn't test it on ``castor``), you will be out of luck.

Submissions are made by committing files and pushing the commits to the assignment repo on GitHub.

For Task 4, you will receive credit only if you correctly completed Task 3.

As with all work in CPEN212, everything you submit must be original and written by you from scratch after the lab has been released. You may not refer to any code other than what is found in the textbooks and lectures, and you must not share code with anyone at any time. See the academic integrity policy for details.

Prior to starting work on this lab, you **must** secure your home directory as described on Canvas; otherwise we will consider you to have shared your solution code with the rest of the class.



Task 1
======

This is essentially a brief tutorial. You will learn to disassemble executable code and follow its execution in a debugger.


Your very own test program
--------------------------

You will get your very own ``task1`` test program. To generate it, run::

    ~cpen212/Public/lab1/task1gen

while logged in to ``castor.ece.ubc.ca``.

This will create a ``task1`` executable in the current directory (so you better have permissions there, or it will fail).

The program accepts one command-line argument, displays a message, and exits. It is almost entirely code we've seen in lecture, with the exception of the part that extracts the command-line argument.

Run ``task1`` with one argument to see what it does; also run it with no arguments and more than one argument to see what happens.


Disassembly
-----------

The first thing we will do is look at the bytes in the ``task1`` binary. To do so, run::

    xxd task1

You should see the bytes that constitute the ``task1`` program, including some of the text it displays. Note that the leftmost column is the *byte offsets in the file*, not the addresses at which those offsets will be loaded in memory.

But what instructions do these correspond to? Let's disassemble this program::

    objdump -d task1

Chances are you probably won't see any coherent output, because ``objdump`` doesn't know where to begin decoding. To fix that, give ``objdump`` a starting address to look at::

    objdump -d --start-address=0xADDRESS task1

**Hint**: you can find the starting address from gdb (vide infra).

Once you get past this hurdle, you will see mostly real instructions; for each, you will see the address *at which the instruction will be loaded in memory*, the 32-bit word that encodes the instruction, and the actual instruction. For some of these, though, the instruction is undefined or very weird (like ``fnmls``). What are these?

Let's look at the instruction encoded as ``6c6c6568``. Can you find these bytes in the ``xxd`` output? Why is this being decoded as an instruction by ``objdump``?

If you're having trouble finding it, remember that ARM64 is **little-endian**, which means that the bytes in the program will appear in the reverse order compared to when they are written as one 32-bit (or 64-bit) word. For example, the 64-bit number ``decafc0ffee15bad`` (hexadecimal) on a little-endian machine would be stored with the least significant byte (``ad``) at the lowest address, so if you read the memory in order one byte at a time you would see ``ad 5b e1 fe 0f fc ca de``.


Debugging
---------

Now, let's run ``task1`` in the GDB debugger and see exactly what it's doing::

    gdb --args task1 someargument

(you may need to provide the correct path for ``task1``).

Normally we would set a breakpoint at some known label and run the program until it hits that breakpoint. Inconveniently, however, it seems that someone stripped all of the symbols from this program, so ``gdb`` doesn't know where any of the labels were. How inconsiderate of them.

Instead, let's ask GDB to stop the program at the first instruction::

    starti

When you do this, GDB should stop when the program starts and show you the PC. Let's look at what code we're about to run::

    disas $pc, +16

This +16 means the next 16 bytes following the PC. You can of course disassemble more bytes, or use an actual address rather than ``$pc``.

To step through the program, use::

    si

which steps through one instruction at a time, and follows function calls (``bl``). ``ni`` also steps one instruction at a time, but executes any function called with ``bl`` as if it were one step, without single-stepping through the instructions inside.

You can also display the values of registers::

    p/x $sp
    p/x $x0

(``x`` means display in hex) and the top of the stack::

    x/8xg $sp

(``8xg`` means display eight hex-formatted 64-bit values). If you instead use ``display``::

    display/x {$pc, $sp, $x0}

then the values will be automatically displayed after every debugger step.

This is the bare minimum of commands you need to know to get started. GDB has many other commands, and there are tons of resources on the web that describe them.


Questions
---------

In the ``task1`` folder, you will find a file called ``task1.h``, which asks several questions about your ``task1``.  They all ask about addresses of specific things inside the ``task1`` process. Note, when we ask about the address of the first function vs. the second function, the first function would be at a lower address than the second.

To complete the answers, modify the corresponding C file (``task1.c``) so that each function returns the answer to the corresponding question as defined in ``task1.h``, commit, and push to the GitHub repo to submit.

The ``task1`` folder contains sources for a ``check`` program you can use to validate that your answers are in the correct *format* by displaying their values; you can run ``make`` to build ``check``. We will use our own equivalent of ``check`` to mark your submission and ignore any modifications you make to the ``.c`` and ``.s`` files it consists of, so you should make sure that your ``task1.c`` works with the original ``check`` code.

All of the code you submit lab must be side-effect-free, and must compile, link, and execute on "bare metal," that is without linking against C standard libraries or the C runtime. The example makefile complies with this, so if your code runs inside ``check`` without modifying the makefile, you should be all set.


Deliverables
------------

- ``task1/task1.c``



Task 2
======

Time to test your skills on a harder version of ``task1``. This time not only the labels are missing, but someone invented their own calling convention, which you will also have to reverse-engineer.


Your very own ``task2``
-----------------------

You will get your very own ``task2``. To generate it, run::

    ~cpen212/Public/lab1/task2gen

This will create a ``task2`` in the current directory (so you better have permissions there, or it will fail).

This also accepts one argument. This argument is a password, and ``task2`` will either print ``PASSED`` or ``FAILED`` depending on whether you've provided the correct password.


Analysis
--------

In this and the next task, you will use the skills you learned in Task 1 to determine how ``task2`` checks its input.

For this task, start by identifying all functions and the addresses like in Task 1. Then determine as much as you can about the calling convention being used by the program (argument and return registers, caller-saved and callee-saved registers, etc). Note that the calling convention being used is likely different from what we covered in class.

Finally, there are four checks that the program performs on the password it receives. If any of these fail, the program reports ``FAILED``, and if all four pass, then the program reports ``PASSED``.  Two of these checks are each contained entirely in a function. For those checks write down the address of the function. The other two checks call a function and then check the return value of that function. For these checks —- write down the address of the first function the check calls. 

For example, if a check semantically looks like this (remember, it won't *really* look like this, because you'll see the assembly)::

    ret = foo(string);
    if (ret == SOME_NUMBER)
        return 1; /* or jump to one location */
    else
        return 0; /* or jump to another location */

then you write down the address of foo().

Once you determined the functions corresponding to each of those checks, sort them according to their addresses. So check 1 will be have the lowest address, and check 4 will have the highest. You will need to write down these addresses in your submission.

As before, there is a ``task2/task2.c`` file you need to fill out with the corresponding questions in ``task2/task2.h``. Also as before, your code must compile, link, and execute on bare metal, and may not have any side effects. Be sure to test your code — for example by adaptng the ``check`` program from Task 1 — as you will receive no credit if the program doesn't compile, crashes, or returns incorrect results.


Deliverables
------------

- ``task2/task2.c``



Task 3
======

More fun with reverse-engineering ``task2``.


Analysis
--------

Reverse-engineer each of the four password checks. This time, most of the functions are *not* from lecture, so you will need to figure out the functionality from the disassembled code stepping through the program in GDB.

Once you know what each check does, implement a C function that performs exactly the same check as the corresponding code in the ``task2`` executable: that is, given the same input string, it must return the same number determining whether the check passed. The semantics of each function should be: if a check passes, the function returns '1', if it fails the function returns '0'. (Semantics is a fancy word for meaning.) As before, two of the checks in ``task2`` are contained entirely within a function and already follow the required semantics. But the other two checks call a function to compute a value and then compare that value against the right answer elsewhere. **Make sure that those other two checks are implemented such that they perform the check AND return 0 or 1.**

There is a ``task3/task3.c`` file you need to fill out with the corresponding reverse-engineered functions, in the order in which they appear in memory when ``task2`` is being executed.

Remember that your code must compile, link, and execute on bare metal, and may not have any global side effects. This means that your code can't use any libc functions (like ``strlen``, ``strcmp``, and so on).

To verify that your checks are implemented correctly, come up with a test string, and step through ``task2`` to see what each given check returns. Then see if your implementation returns the same thing. **Hint**: you can have ``gdb`` jump to a specific function as follows::

    jump *ADDRESS

Just make sure that you do that after the program has prepared the arguments in a way that the function expects. 

Deliverables
------------

- ``task3/task3.c``


Task 4
======

Three of the four checks should give you a pretty good idea about what constraints the password needs to satisfy. The remaining constraint is harder to convert into a usable constraint (it's possible, but tricky), but the other three already reduce the possibilities space so much that it's easiest to enumerate the remaining options and check each of them against this remaining check.

Implement code that uses your reverse-engineered function(s) from Task 3 to search through the password space and find a working password.

Once you have found a password that works, enter it in ``task4/PASSWORD`` **without any extra characters** (no spaces, quotation marks, newlines, etc.). If the password is correct, you should be able to get your ``task2`` program to display ``PASSED`` using a command like this::

    cat task4/PASSWORD | xargs ./task2/task2

(this will vary a bit depending on where you generated the ``task2`` binary).

For this task, **you will only receive credit if you have correctly reverse-engineered all four checks in Task 3**.


Deliverables
------------

- ``task4/PASSWORD``



Bonus
=====

In this task you will investigate what happens when the indirection provided by the ``bl`` and ``ret`` instructions is not available in an ISA, and discover how it would be possible to work around this.

The following code implements the Ackermann function::

    unsigned long ack(unsigned long m, unsigned long n) {
        if (m == 0) return n + 1;
        if (n == 0) return ack(m-1, 1);
        return ack(m-1, ack(m, n-1));
    }

This function is mainly interesting because it is a simple function that is not *primitive recursive*, meaning that (very informally) the recursion cannot be turned into a bunch of simple loops.

Your task is to implement the ``ack`` function with exactly the same semantics as the C code above (including overflow) **without using indirect jumps and return instructions**.

This means that you cannot use any instruction that transfers control flow to a value taken from any register. This includes the ``ret`` instruction (which jumps to the address in the link register ``x30``) as well as instructions that jump to the address in some other register (e.g., ``br``, ``blr``, and their variants).

The only control flow instructions you may use are PC-relative branches which encode the offset from the current PC *in the instruction itself*. These include unconditional branches like ``b``, conditional branches like ``beq``, and combined compare-and-branch instructions like ``cbz``.

How could you possibly return from anywhere without an indirect-jump instruction like ``ret``, though?

Well, if a function is only called from one place, this is easy::

    ...your code...
    b return_label

The tricky part is if this function can be called from multiple places, including code that didn't exist when the function was written. What then?

But actually, every time we call this function, we can figure out where it should return from the call we are about to make, and *construct a branch instruction* that has the appropriate offset. We already saw in the first lecture how to put the address of something in a register, so all we need to do is compute the offset and create the instruction encoding for a ``b`` instruction that jumps to the right place.

This technique is called a Wheeler jump (after computing pioneer David Wheeler) and is an example of self-modifying code.

Fill in the definition of ``ack`` in ``bonus/bonus.s`` so that it obeys these constraints (i.e., no register-indirect jumps).


Deliverables
------------

- ``bonus/bonus.s``



Marks
=====

To earn marks, you must commit and push each task to the GitHub repo **before the deadline for that task**.

Remember that CPEN 212 labs are **individual**, so you must complete all tasks by yourself; see the academic integrity policy for details.

- Task 1: 2 mark
- Task 2: 3 marks
- Task 3: 3 marks
- Task 4: 2 marks
- Bonus: 1 mark

