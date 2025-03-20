*********
1337 h4xx
*********

.. contents:: Contents
    :depth: 2


TL;DR
=====

Deadlines
---------

- Tasks 2 and 3: January 28th, 23:59 Vancouver time.

- Tasks 4–6: February 4th, 23:59 Vancouver time.


Logistics
---------

As in prior labs, you will use ``castor.ece.ubc.ca`` to complete this lab; we will also use ``castor`` to evaluate your submission. As usual, submit by pushing to the GitHub repo.

Make sure you understand arm64 assembly coding, the arm64 calling convention, and all of the attacks in the haxx lectures.

As with all work in CPEN212, everything you submit must be original and written by you from scratch after the lab has been released. You may not refer to any code other than what is found in the textbooks and lectures, and you must not share code with anyone at any time. See the academic integrity policy for details.

Task 1 is basically a tutorial, so do that first; **don't skip it** or you will have a hard time with the later tasks. Also, you might want to read the hints.


Hackable executable
-------------------

For tasks 2 to 6, you will use an ``rpn`` executable generated for you by::

    ~cpen212/Public/lab2/rpngen

You will need to run the resulting executable with either ``setarch`` or ``gdb`` to disable stack randomization; see below.



Task 1: Hacking basics
======================

In this task, you will create and analyze three types of exploits on a simple program to learn the ropes.


Simple victim program
---------------------

In the ``task1`` folder you will find the program ``swallow``, which will be our target for the tutorial part of the lab.

To run it (and hack it), we will need to disable address space layout randomization (ASLR), which would randomize the initial stack location as we discussed in lecture. You can do so running the program like this::

    setarch -R ./swallow

Conveniently, ``gdb`` also disables ASLR, so we can run and examine ``swallow`` in ``gdb``::
    
    gdb ./swallow

If you run it without disabling ASLR, you will likely get a segmentation fault for exploits that depend on stack addresses etc. You will need to run programs via ``setarch -R`` or ``gdb`` for the rest of the lab.

Run the program and see what it does. (European swallows fly at around 20 to 30 knots, if you were curious, but the program does not care what you type in.)

Find the key function of ``swallow`` under `Sources`_. You can see that it uses ``gets`` just like the example in lecture; we will again abuse this to hack the program.

Your aim will be to call one of three functions, ``pwn1``, ``pwn2``, and ``pwn3``, also part of ``swallow``, and also shown in `Sources`_. They just create files with specific names, either empty or with specific contents.


Preparing inputs
----------------

You will eventually need to craft inputs to feed to the program you want to hack.

You can of course use an editor to create a text file (say ``file``), and then feed it to ``swallow`` like this::
    
    setarch -R ./swallow < file

Try it out on a very short text file.

To perform most hacks, you will need to create files with arbitrary bytes, not just text. One way to do this is to use the ``xxd`` command in reverse mode, so that it takes a bunch of hex numbers and generates a file with the corresponding sequence of bytes. For example if you have a file like this::

    32 35 20 6b 74 73 0a  // this is a comment

then you can run something like::

    xxd -r -p input.hex > output.bin

to convert this to bytes.

You can also run ``xxd`` without any options to see the bytes in any file as hex numbers, like this::
    
    xxd output.bin

Create a file with a sequence of bytes of your choice (the ``gets`` in ``swallow`` will read everything up to ``0x0a``) and verify that the bytes in the file you created correspond to the values that you expect. Feed the binary file to ``swallow`` and see what happens.


Finding addresses
-----------------

You will also need to find the addresses of functions, such as, for example, ``pwn1``. You can do this in various ways; one is to use ``objdump`` to dump the symbol table::

    objdump -t swallow

To look for some text like ``pwn1`` you can feed the output of ``objdump`` through ``grep`` like this::

    objdump -t swallow | grep pwn1

you should see::

    <address-of-pwn1> g     F .text	<size-of-pwn-1> pwn1

(The ``g`` here indicates that this is a global symbol, and ``F`` that it is a function.)

Find the addresses of ``pwn1``, ``pwn2``, and ``pwn3`` in the ``swallow`` binary.

Instead of ``objdump -t`` you can also use ``readelf -s``, which outputs this information with slightly different formatting.


Dissecting the stack structure
------------------------------

This is best done either by looking at the assembly code of the function (e.g., via ``objdump -d``) or by single-stepping with ``gdb``, as you did in a prior lab.

In ``gdb``, you might find it convenient to set **watchpoints** so that you know when particular stack locations get overwritten, and/or when they are read.

The best approach here is to draw a picture of what is on the stack and how much space it takes while you go through the code, just like we did in lecture.

You can also find out the stack address by running the program under ``gdb``. Normally, ``gdb`` would give you a different address than running the program by itself, but for this lab we have done some tricks to make the stack addresses stable.

Examine the ``swallow`` function. Find the address of ``speed`` on the stack, and the absolute address where the return addresses (saved ``lr`` values) of (i) ``swallow`` and (ii) ``main212`` are stored.


Finding ROP gadgets
-------------------

For Return-Oriented Programming, you will need to find short sequences of instructions that you can "reuse" for an exploit. Most often, this is one or two instructions that load register values from the stack followed by ``ret``, although it can also be an indirect jump like ``br x1`` etc.

Again, we will start by disassembling ``swallow`` with ``objdump -d``::

    objdump -d swallow > swallow_code.txt

This is a lot of code. But we're only interested in sequences of instructions that end in ``ret``. We can use our old friend ``grep`` to select those, by asking it to find instances of ``ret`` in ``swallow_code.txt`` and print each together with the four preceding lines::

    grep -B4 ret swallow_code.txt > swallow_rets.txt

(The ``-B`` with a number says to print that many lines before the match.)

At this point it's feasible to search through the instruction fragments in an editor and look for instructions of interest (e.g., a stack-relative ``ldr``). Refer to the lecture for the kind of sequences you might want to find.

Of course you could automate this further (e.g., by using multi-line regular expressions), but unless you already know how to do that it'll probably take you longer than doing it by hand, and you will likely miss some interesting ROPs.

Find the location of a sequence of instructions in ``swallow`` which:
    
- loads a value from some stack offset into ``x1``,
- loads the value of ``x30`` from the stack, and
- returns.

There can be some other *intervening instructions*, but they should not affect ``x1`` or ``x30``. Now, you've found an ROP gadget you might be able to use to pick up the second function argument off the stack... that could come in useful later.


Analyzing exploits
------------------

You will find some exploits in ``flight1.hex``, ``flight2.hex``, and ``flight3.hex``. Make sure you understand what they do and how they work; you will be creating similar exploits in the remaining tasks.

Pay attention to the file format. At the end of each line, you can add a comment: ``//`` followed by text that explains what the line does. You will need to do that for the exploits you craft in the remaining tasks.


Hints
-----

- You might find ``gdb`` commands similar to the following useful:

  - ``set disassemble-next-line on``
  - ``display /x {$x0, $x1, $x30}``
  - ``display /16xg $sp``

- In lecture we used loads relative to ``sp``, but often you know the values of other registers, such as ``x29``.

- In lecture we used ``ret`` to effect the control flow transfers. But really this can be any indirect jump through any register we pick up from the stack.



Task 2: Hijacking the return address
====================================

Now it's your turn to hack some programs.

This program is a simple (and buggy!) reverse Polish notation calculator. You don't need to understand RPN, but for fun you can run::
    
    setarch -R rpn 6fact.rpn
    setarch -R rpn e.rpn

(with paths before ``rpn`` and ``e.rpn`` etc. pro re nata) to see some examples of how it works. For technical reasons™ involving making the code easier for you to hack, RPN commands are executed only if followed by a space character.

You can find the relevant source code in `Sources`_. If you read the ``read_tok`` function, you will see that it reads bytes into a buffer (``tok``) without worrying about the size of ``tok``. This means that we can use this to overflow ``tok`` and hijack the program!

Craft shellcode in ``task2.hex`` that causes your version of ``rpn`` to call ``pwn1()`` (which creates a new empty file called ``pwn3d``).

To receive credit, your shellcode **must** contain an explanation of what each line does, in the format discussed in `Analyzing exploits`_.

We will evaluate your exploit by running::
    
    xxd -r -p task2.hex > task2.bin
    setarch -R rpn task2.bin

where ``rpn`` will be the path to the version of the program assigned to you, and ``task2`` will be replaced by ``task3``, ``task4``, ``task5``, and ``task6`` for the remaining tasks.

In comments inside ``task2.hex``, describe each part of your exploit in the format described above.


Hints
-----

- You will need to find out how much space to fill between the beginning of ``tok`` and the return address. Finding where the return address is is pretty easy if you set ``gdb`` to break at the various functions and look at the values of the stack pointer in the first few instructions of each function.

- Examining a few values on the top of the stack as well as individual registers is invaluable to check whether / how your exploit is working.

- What about ``tok``? If you look at the source, you will see that ``tok`` is written soon after a call to some variant of ``fgetc``. You can either read the code or watch execution in ``gdb`` to get some intuition about how you can abuse ``tok``.

- You can also set a watchpoint where the return address on the stack should be, and check that the watchpoint is tripped when you provide your exploit input.


Deliverables
------------

- ``task2/task2.hex`` with your exploit and analysis



Task 3: Stack code injection I
==============================

Craft shellcode in ``task3.hex`` that uses *stack code injection* and causes your assigned version of ``rpn`` to call ``pwn2()`` with argument ``30`` (number); this will create a new empty file called ``pwn3d-030``.

We well test your code as above, except that we will also make sure the exploit stops working if the stack is set to non-executable in the binary (to distinguish it from ROP attacks).


Deliverables
------------

- ``task3/task3.hex`` with your exploit and analysis



Task 4: Stack code injection II
===============================

Craft shellcode in ``task4.hex`` that uses *stack code injection* and causes your assigned version of ``rpn`` to call ``pwn3()`` with arguments ``40`` (number) and ``l33th4x0r`` (string); this will create a new file called ``pwn3d-040`` with the contents from the second argument.

We well test your code as above, except that we will also make sure the exploit stops working if the stack is set to non-executable in the binary.


Hints
-----

- Remember that strings in C are just addresses of the first character in some sequence, so you need to pass an address to it as we did in lecture.

- To find out the encoding of some assembly instructions, you can use ``as`` and disassemble the resulting object file with ``objdump``. (You can of course encode instructions by hand as well if you really have nothing better to do with your time.)

- Use an indirect branch instruction to go to a specific address that you first load in the relevant register. Review the lecture for how to load large constants into registers.

- You want to place the instructions and the string somewhere *above in the address space* than the return address location (i.e., at a higher address), so that it is not within the parent function's stack frame. The reason is that when you call ``pwn3()``, it will also extend the stack downward, and might overwrite your carefully prepared string.


Deliverables
------------

- ``task4/task4.hex`` with your exploit and analysis



Task 5: Return-Oriented Programming I
=====================================

Craft shellcode in ``task5.hex`` that uses *return-oriented programming* and causes your assigned version of ``rpn`` to call ``pwn2()`` with argument ``50`` (number); this will create a new empty file called ``pwn3d-050``.

We well test your code as above, except that we will also make sure the exploit works whether or not the stack is set to non-executable in the binary (to distinguish it from stack code injection).

Deliverables
------------

- ``task5/task5.hex`` with your exploit and analysis



Task 6: Return-Oriented Programming II
======================================

Craft shellcode in ``task6.hex`` that uses *return-oriented programming* and causes your assigned version of ``rpn`` to call ``pwn3()`` with arguments ``60`` (number) and ``ph33rm3n00bz`` (string); this will create a new file called ``pwn3d-060`` with the contents from the second argument.

We well test your code as above, except that we will also make sure the exploit works whether or not the stack is set to non-executable in the binary (to distinguish it from stack code injection).


Hints
-----

- If you can't find a suitable gadget to write to ``x1``, see `Finding ROP gadgets`_.


Deliverables
------------

- ``task6/task6.hex`` with your exploit and analysis



Marks
=====

To earn marks, you must commit and push each task to the GitHub repo **before the deadline for that task**.

Remember that CPEN 212 labs are **individual**, so you must complete all tasks by yourself; see the academic integrity policy for details.

- Task 1: 0 marks
- Task 2: 2 marks
- Task 3: 2 marks
- Task 4: 2 marks
- Task 5: 2 marks
- Task 6: 2 marks



Sources
=======

Swallow
-------

Here is the ``swallow`` function::

    void swallow() {
        char speed[32];
        printf("what is the airspeed velocity of an unladen swallow? ");
        gets(speed);
        printf("swallows fly at: %s\n", speed);
    }


PWN functions
-------------

These are the functions you will call for the various exploits; they are defined in both ``swallow`` and ``rpn``::

    void pwn1() {
        FILE *f = fopen("pwn3d", "w");
        if (f) fclose(f);
    }
    
    void pwn2(uint8_t n) {
        char name[10];
        strcpy(name, "pwn3d-");
        name[6] = '0' + (n / 100);
        name[7] = '0' + ((n % 100) / 10);
        name[8] = '0' + (n % 10);
        name[9] = '\0';
        FILE *f = fopen(name, "w");
        if (f) fclose(f);
    }
    
    void pwn3(uint8_t n, const char *s) {
        char name[10];
        strcpy(name, "pwn3d-");
        name[6] = '0' + (n / 100);
        name[7] = '0' + ((n % 100) / 10);
        name[8] = '0' + (n % 10);
        name[9] = '\0';
        FILE *f = fopen(name, "w");
        if (f) {
            fputs(s, f);
            fclose(f);
        }
    }


RPN
---

Code below; you really only need to worry about the ``read_tok()`` function::
    
    typedef struct cell_s {
        double val;
        struct cell_s *prev;
    } cell_t;
    
    typedef struct {
        const char *op;
        void (*fn)(cell_t **stack);
    } opdesc_t;
    
    void die(const char *msg1, const char *msg2) {
        if (msg2) fprintf(stderr, "ERROR: %s: %s\n", msg1, msg2);
        else fprintf(stderr, "ERROR: %s\n", msg1);
        exit(1);
    }
    
    static void push(cell_t **stack, double val) {
        cell_t *cell = malloc(sizeof(cell_t));
        cell->val = val;
        cell->prev = *stack;
        *stack = cell;
    }
    
    static double pop(cell_t **stack) {
        cell_t *cell = *stack;
        if (!cell) die("empty stack", NULL);
        double val = cell->val;
        *stack = cell->prev;
        free(cell);
        return val;
    }
    
    static void eval_add(cell_t **stack) { push(stack, pop(stack) + pop(stack)); }
    static void eval_sub(cell_t **stack) { push(stack, pop(stack) - pop(stack)); }
    static void eval_mul(cell_t **stack) { push(stack, pop(stack) * pop(stack)); }
    static void eval_div(cell_t **stack) { push(stack, pop(stack) / pop(stack)); }
    static void eval_pow(cell_t **stack) { push(stack, pow(pop(stack), pop(stack))); }
    static void eval_dup(cell_t **stack) { double v = pop(stack); push(stack, v); push(stack, v); }
    static void eval_prn(cell_t **stack) { double v = pop(stack); printf("%f\n", v); }
    
    const opdesc_t ops[] = {
            { "+", eval_add },
            { "-", eval_sub },
            { "*", eval_mul },
            { "/", eval_div },
            { "^", eval_pow },
            { ":", eval_dup },
            { ".", eval_prn },
            { NULL, NULL }
    };
    
    void eval_tok(cell_t **stack, char *tok) {
        for (const opdesc_t *op = ops; op->op && op->fn; ++op) {
            if (strcmp(tok, op->op) == 0) {
                op->fn(stack);
                return;
            }
        }
        double val = strtod(tok, &tok);
        push(stack, val);
    }
    
    int read_tok(cell_t **stack, FILE *f, const char *fn) {
        char tok[TOKBUF];
        register int i = 0;
        register int c;
        do {
            c = fgetc(f);
            tok[i++] = c;
        } while (c != EOF && c != ' ');
        tok[i-1] = 0;
        if (c != EOF)
            eval_tok(stack, tok);
        else if (ferror(f))
            die(fn, strerror(errno));
        return (c != EOF);
    }
    
    void read_file(cell_t **stack, const char *fn) {
        FILE *f = fopen(fn, "r");
        if (!f) die(fn, strerror(errno));
        while (read_tok(stack, f, fn));
    }
    
    int main212(int argc, char **argv) {
        cell_t *stack = NULL;
        for (int i = 1; i < argc; ++i) {
            read_file(&stack, argv[i]);
        };
        return 0;
    }

The main function is called ``main212`` instead of ``main`` because we did some hacks to ensure that the stack starts in a predictable place for marking; otherwise it depends on things like your shell environment variables and so on, and we might not be able to reproduce your answers. You don't need to worry about this — you can treat ``main212`` like a regular ``main`` function.
