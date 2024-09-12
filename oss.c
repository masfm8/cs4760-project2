#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include "shared_memory.h"

#define MAX_PROCESSES 20

// Simulated clock variables
int *shared_clock_sec;
int *shared_clock_ns;

// Process Control Block (PCB) structure
struct PCB {
    int occupied;
    pid_t pid;
    int startSeconds;
    int startNano;
};

struct PCB processTable[MAX_PROCESSES];

// Function to clean up shared memory and terminate child processes
void cleanup_and_exit(int sig) {
    // Send kill signal to all child processes, free shared memory
    shmctl(shm_id, IPC_RMID, NULL);
    exit(0);
}

int main(int argc, char *argv[]) {
    signal(SIGALRM, cleanup_and_exit); // Set up termination after 60 seconds
    alarm(60);  // Alarm in 60 seconds
    
    // Shared memory setup
    setup_shared_memory();
    
    // Example loop for launching processes and incrementing clock (this will evolve)
    while (1) {
        increment_clock();
        check_for_termination();
        fork_worker_process();
    }
    
    return 0;
}
