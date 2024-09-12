CC = gcc
CFLAGS = -Wall

all: oss worker

oss: oss.o shared_memory.o
    $(CC) $(CFLAGS) -o oss oss.o shared_memory.o

worker: worker.o shared_memory.o
    $(CC) $(CFLAGS) -o worker worker.o shared_memory.o

oss.o: oss.c shared_memory.h
    $(CC) $(CFLAGS) -c oss.c

worker.o: worker.c shared_memory.h
    $(CC) $(CFLAGS) -c worker.c

shared_memory.o: shared_memory.c shared_memory.h
    $(CC) $(CFLAGS) -c shared_memory.c

clean:
    rm -f *.o oss worker
