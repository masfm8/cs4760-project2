#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shared_memory.h"

int shm_id;
key_t key = 859047; // Shared memory key

void setup_shared_memory() {
    // Create shared memory for clock
    shm_id = shmget(key, sizeof(int) * 2, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(1);
    }
}

void attach_shared_memory(int **shared_clock_sec, int **shared_clock_ns) {
    // Attach to shared memory
    int *shm_ptr = (int *)shmat(shm_id, NULL, 0);
    *shared_clock_sec = shm_ptr;
    *shared_clock_ns = shm_ptr + 1;
}

void detach_shared_memory() {
    shmdt(shared_clock_sec);
    shmdt(shared_clock_ns);
}
