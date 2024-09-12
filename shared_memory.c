#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shared_memory.h"

#define SHM_KEY 0x12345
#define SHM_SIZE sizeof(int) * 2  // Two integers for seconds and nanoseconds

static int shm_id;
static int *system_clock;

void init_shared_memory() {
    shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget");
        exit(1);
    }
    system_clock = (int *)shmat(shm_id, NULL, 0);
    if (system_clock == (int *) -1) {
        perror("shmat");
        exit(1);
    }
    system_clock[0] = 0;  // Seconds
    system_clock[1] = 0;  // Nanoseconds
}

int *attach_shared_memory() {
    int *shm_ptr = (int *) shmat(shm_id, NULL, 0);
    if (shm_ptr == (int *) -1) {
        perror("shmat");
        exit(1);
    }
    return shm_ptr;
}

void detach_shared_memory() {
    if (shmdt(system_clock) == -1) {
        perror("shmdt");
    }
}

void cleanup_shared_memory() {
    shmctl(shm_id, IPC_RMID, NULL);
}
