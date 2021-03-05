CC=g++
CFLAGS=-I. -std=c++17 -march=native -static
DEP=
EXEC=bomb


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC): bomb.o
	$(CC) -o $(EXEC) bomb.o

clean:
	rm $(EXEC) *.o
