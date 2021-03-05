.PHONY: all clean
CC=g++
CFLAGS=-I. -std=c++17 -march=native -static -g
TARGET=tower tower_data

all: $(TARGET)

tower: tower.c
	$(CC) -o $@ $^ $(CFLAGS)

tower_data: tower_data.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(TARGET)
