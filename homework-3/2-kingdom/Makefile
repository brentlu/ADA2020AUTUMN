.PHONY: all clean
CC=g++
CFLAGS=-I. -std=c++17 -march=native -static -g
TARGET=kingdom kingdom_data

all: $(TARGET)

kingdom: kingdom.c
	$(CC) -o $@ $^ $(CFLAGS)

kingdom_data: kingdom_data.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(TARGET)
