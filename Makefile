CC = gcc
CFLAGS = -Wall

all: oss worker

oss: oss.c
	$(CC) $(CFLAGS) oss.c -o oss

worker: worker.c
	$(CC) $(CFLAGS) worker.c -o worker

clean:
	rm -f oss worker
