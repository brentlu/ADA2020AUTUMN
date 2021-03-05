CC=g++
CFLAGS=-I. -std=c++17 -march=native -static
DEP=
EXEC=robot


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC): robot.o
	$(CC) -o $(EXEC) robot.o

clean:
	rm $(EXEC) *.o
