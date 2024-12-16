#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <stdbool.h>

#define SHMKEY 9876

int shmid;
int *shmClock;

// Signal handler for SIGINT
void sig_handler(int signo) {
    if (signo == SIGINT) {
        shmdt(shmClock);
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sig_handler); // Handle SIGINT to clean up

    if (argc < 2) {
        fprintf(stderr, "WORKER: Missing time to stay argument.\n");
        exit(1);
    }

    int retries = 5;
    while (retries--) {
        shmid = shmget(SHMKEY, 2 * sizeof(int), 0666);
        if (shmid != -1) break;
        usleep(500000); // Retry delay
    }
    if (shmid == -1) {
        fprintf(stderr, "WORKER: Failed to get shared memory.\n");
        exit(1);
    }

    shmClock = (int *)shmat(shmid, NULL, 0);
    if (shmClock == (int *)-1) {
        fprintf(stderr, "WORKER: Failed to attach to shared memory.\n");
        exit(1);
    }

    int secondsToStay = atoi(argv[1]);
    printf("WORKER PID: %d attached to shared memory.\n", getpid());

    while (shmClock[0] < secondsToStay) {
        usleep(1000);
    }

    shmdt(shmClock);
    printf("WORKER PID: %d exiting.\n", getpid());
    return 0;
}
