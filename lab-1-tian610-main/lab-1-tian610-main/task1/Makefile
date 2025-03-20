CFLAGS=-nostdlib -fno-builtin

check: check.c task1.c task1lib.s task1.h task1lib.h
	cc $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f check *.o
