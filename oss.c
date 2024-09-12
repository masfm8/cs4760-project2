#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shared_memory.h"

#define MAX_PROCESSES 20

struct PCB {
    int occupied;  // 0 for free, 1 for occupied
    pid_t pid;     // Child process ID
    int startSeconds;
    int startNano;
};

struct PCB processTable[MAX_PROCESSES];

void signal_handler(int sig) {
    // Terminate all child processes and clean up
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processTable[i].occupied) {
            kill(processTable[i].pid, SIGKILL);
        }
    }
    cleanup_shared_memory();
    exit(0);
}

int main(int argc, char *argv[]) {
    // Initialize shared memory and system clock
    init_shared_memory();
    signal(SIGALRM, signal_handler);
    alarm(60);  // Set a 60-second alarm for termination

    // Your logic for process management here (fork, exec workers, etc.)

    // Clean up
    cleanup_shared_memory();
    return 0;
}
