CC=g++
CFLAGS=-I. -std=c++17 -march=native -static
DEP=
EXEC=party


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC): party.o
	$(CC) -o $(EXEC) party.o

clean:
	rm $(EXEC) *.o
