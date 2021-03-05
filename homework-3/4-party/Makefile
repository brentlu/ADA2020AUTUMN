.PHONY: all clean
CC=g++
CFLAGS=-I. -std=c++17 -march=native -static -g
TARGET=party party_data

all: $(TARGET)

party: party.c
	$(CC) -o $@ $^ $(CFLAGS)

party_data: party_data.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(TARGET)
