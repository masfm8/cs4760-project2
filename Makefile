CC = gcc
CFLAGS = -Wall -g
OBJ = oss.o worker.o shared_memory.o
DEPS = shared_memory.h

all: oss worker

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

oss: oss.o shared_memory.o
	$(CC) -o oss oss.o shared_memory.o $(CFLAGS)

worker: worker.o shared_memory.o
	$(CC) -o worker worker.o shared_memory.o $(CFLAGS)

clean:
	rm -f *.o oss worker
