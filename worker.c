#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shared_memory.h"

int main(int argc, char *argv[]) {
    // Attach to shared memory for system clock
    int *shared_clock_sec, *shared_clock_ns;
    attach_shared_memory(&shared_clock_sec, &shared_clock_ns);
    
    // Fetch max time to stay in the system (from command line)
    int max_sec = atoi(argv[1]);
    int max_ns = atoi(argv[2]);

    // Calculate termination time based on shared clock
    int term_sec = *shared_clock_sec + max_sec;
    int term_ns = *shared_clock_ns + max_ns;

    while (1) {
        if (*shared_clock_sec >= term_sec && *shared_clock_ns >= term_ns) {
            printf("Worker terminating\n");
            break;
        }
    }

    return 0;
}
