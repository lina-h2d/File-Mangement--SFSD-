# Compiler and flags
CC = gcc
CFLAGS = -Wall -I/opt/homebrew/Cellar/raylib/5.5/include -I../include
LDFLAGS = -L/opt/homebrew/Cellar/raylib/5.5/lib -lraylib -lm

# Source files
SRC = raylib_interface.c ../src/file.c ../src/memory.c

# Output executable
TARGET = raylib_interface

# Default target
all: $(TARGET)

# Rule to build the target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Clean up build files
clean:
	rm -f $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean run