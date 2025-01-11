# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread

# Default source files (if no input is provided by the user)
DEFAULT_SRC = TCP_socket.c connect_manager.c main.c

# User-specified source files (if provided during `make` invocation)
SRC = $(DEFAULT_SRC) $(USER_SRC)

# Object files
OBJ = $(SRC:.c=.o)

# Name of the final executable
TARGET = chat_app

# Default target to build the executable
all: $(TARGET)

# Rule to link object files and create the executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(CFLAGS)

# Rule to compile .c files into .o object files
%.o: %.c
	$(CC) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f *.o *.elf

# Optional: Make the executable `rebuild` (clean and build again)
rebuild: clean all
