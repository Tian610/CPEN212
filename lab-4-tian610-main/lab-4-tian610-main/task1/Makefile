# chat how do i make a makefile :(

# Compiler and flags
CFLAGS = -Wall -g  # Enable all warnings and debugging symbols

# Files
SRC = trie.c trietest.c  # Source files
OBJ = $(SRC:.c=.o)  # Object files (.c -> .o)

# Output binary
TARGET = trietest  # Executable name

# Default target: compile and link the program
all: $(TARGET)

# Rule for building the target
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Rule for compiling trie.c
trie.o: trie.c trie.h
	$(CC) $(CFLAGS) -c trie.c

# Rule for compiling trietest.c
trietest.o: trietest.c trie.h
	$(CC) $(CFLAGS) -c trietest.c

# Clean up object files and executable
clean:
	rm -f $(OBJ) $(TARGET)

# Rebuild everything
rebuild: clean all

.PHONY: all clean run rebuild
