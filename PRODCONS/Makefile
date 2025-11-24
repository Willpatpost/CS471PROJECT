# PRODCONS Makefile
CC = gcc
CFLAGS = -Wall -Wextra -pthread -O2

SRC_DIR = src
BIN_DIR = bin

TARGET = $(BIN_DIR)/PRODCONS
SRC = $(SRC_DIR)/PRODCONS.c
OBJ = $(SRC_DIR)/PRODCONS.o

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(OBJ): $(SRC)
	@mkdir -p $(SRC_DIR)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)

run:
	@echo "Usage: ./bin/PRODCONS <p> <c> <b>"
	@echo "Example: ./bin/PRODCONS 5 5 10"
