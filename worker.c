#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

#define SHMKEY 9876

int shm_id;
int *sharedClock;

void cleanupOnSignal(int sig) {
    if (sig == SIGINT) {
        shmdt(sharedClock);
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, cleanupOnSignal);

    if (argc < 2) {
        fprintf(stderr, "worker: Missing argument for time to stay.\n");
        exit(1);
    }

    shm_id = shmget(SHMKEY, 2 * sizeof(int), 0666);
    if (shm_id == -1) {
        perror("worker: shmget");
        exit(1);
    }

    sharedClock = (int *)shmat(shm_id, NULL, 0);
    if (sharedClock == (int *)-1) {
        perror("worker: shmat");
        exit(1);
    }

    int staySecs = atoi(argv[1]);
    int stayNano = rand() % 1000000000;

    int terminateSec = sharedClock[0] + staySecs;
    int terminateNano = sharedClock[1] + stayNano;
    if (terminateNano >= 1000000000) {
        terminateSec += 1;
        terminateNano -= 1000000000;
    }

    printf("WORKER PID: %d PPID: %d StartS: %ds StartN: %dns TermTimeS: %ds TermTimeN: %dns Starting\n",
           getpid(), getppid(), sharedClock[0], sharedClock[1], terminateSec, terminateNano);

    while (1) {
        if (sharedClock[0] > terminateSec || (sharedClock[0] == terminateSec && sharedClock[1] >= terminateNano)) {
            printf("WORKER PID: %d PPID: %d EndS: %ds EndN: %dns TermTimeS: %ds TermTimeN: %dns Terminating\n",
                   getpid(), getppid(), sharedClock[0], sharedClock[1], terminateSec, terminateNano);
            break;
        }
    }

    shmdt(sharedClock);
    return 0;
}
