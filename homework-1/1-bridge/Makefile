CC=g++
CFLAGS=-I. -std=c++17 -march=native -static -g
DEP=
EXEC=bridge


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC): bridge.o
	$(CC) -o $(EXEC) bridge.o

clean:
	rm $(EXEC) *.o
