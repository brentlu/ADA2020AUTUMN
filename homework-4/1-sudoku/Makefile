.PHONY: all clean
CC=g++
CFLAGS=-I. -std=c++17 -march=native -static -g
TARGET=sudoku sudoku_data

all: $(TARGET)

sudoku: sudoku.c
	$(CC) -o $@ $^ $(CFLAGS)

sudoku_data: sudoku_data.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(TARGET)
