.PHONY: all clean
CC=g++
CFLAGS=-I. -std=c++17 -march=native -static -g
TARGET=treasure treasure_data

all: $(TARGET)

treasure: treasure.c
	$(CC) -o $@ $^ $(CFLAGS)

treasure_data: treasure_data.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(TARGET)
