#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shared_memory.h"

int main(int argc, char *argv[]) {
    // Attach to shared memory
    int *shared_clock = attach_shared_memory();
    
    // Determine how long this worker should live (based on command-line arguments)
    int maxTimeSeconds = atoi(argv[1]);
    int maxTimeNano = atoi(argv[2]);

    // Worker logic: check the shared system clock and exit when time is up

    // Detach from shared memory
    detach_shared_memory();
    return 0;
}
