.PHONY: all clean
CC=g++
CFLAGS=-I. -std=c++17 -march=native -static -g
TARGET=boook boook_data

all: $(TARGET)

boook: boook.c
	$(CC) -o $@ $^ $(CFLAGS)

boook_data: boook_data.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(TARGET)
