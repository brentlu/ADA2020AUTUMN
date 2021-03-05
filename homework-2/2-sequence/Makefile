.PHONY: all clean
CC=g++
CFLAGS=-I. -std=c++17 -march=native -static -g
TARGET=sequence sequence_data

all: $(TARGET)

sequence: sequence.c
	$(CC) -o $@ $^ $(CFLAGS)

sequence_data: sequence_data.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(TARGET)
