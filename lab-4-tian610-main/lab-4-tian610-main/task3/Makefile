# Compiler and flags
CC = gcc
CFLAGS = -g -Wall  # Debugging and warning flags

# Libraries to link
LIBS =  # Add any libraries here if necessary (e.g., -lm for math library)

# Object files
OBJ = cpen212vm.o test.o

# Target for the executable
TARGET = test

# Default target
all: $(TARGET)

# Compile cpen212vm.o from cpen212vm.c
cpen212vm.o: cpen212vm.c cpen212vm.h
	$(CC) $(CFLAGS) -c cpen212vm.c -o cpen212vm.o

# Compile smoketest.o from smoketest.c
test.o: test.c cpen212vm.h
	$(CC) $(CFLAGS) -c test.c -o test.o

# Link the object files to create the final executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS)

# Clean up object files and executable
.PHONY: clean
clean:
	rm -f *.o $(TARGET)
