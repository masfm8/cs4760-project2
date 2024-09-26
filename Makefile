# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Executables
all: oss worker

oss: oss.o
	$(CC) $(CFLAGS) -o oss oss.o

worker: worker.o
	$(CC) $(CFLAGS) -o worker worker.o

# To obtain object files
oss.o: oss.c
	$(CC) $(CFLAGS) -c oss.c

worker.o: worker.c
	$(CC) $(CFLAGS) -c worker.c

# Clean up
clean:
	rm -f *.o oss worker

# Phony targets
.PHONY: all clean

