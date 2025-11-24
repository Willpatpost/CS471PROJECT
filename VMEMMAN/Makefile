# VMEMMAN Makefile
CC = gcc
CFLAGS = -Wall -Wextra -O2

SRC_DIR = src
BIN_DIR = bin

TARGET = $(BIN_DIR)/VMEMMAN
SRC = $(SRC_DIR)/VMEMMAN.c
OBJ = $(SRC_DIR)/VMEMMAN.o

# Default rule
all: $(TARGET)

# Build the binary
$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Compile object file into src/
$(OBJ): $(SRC)
	@mkdir -p $(SRC_DIR)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

# Cleanup
clean:
	rm -f $(OBJ) $(TARGET)

# Helpful run command
run:
	@echo "Usage: ./bin/VMEMMAN <input_file> <page_size> <frames>"
	@echo "Example: ./bin/VMEMMAN sample_input.txt 1024 8"
