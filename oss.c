#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

#define SHMKEY 9876
#define MAX_PROCS 20
#define TIME_INCREMENT 1000000 // Simulated clock increment in nanoseconds

struct ProcessControlBlock {
    int active;
    pid_t pid;
    int startSec;
    int startNano;
};

int shm_id;
int *sharedClock;
struct ProcessControlBlock procTable[MAX_PROCS];

void handleSignal(int signal) {
    if (signal == SIGINT) {
        printf("OSS: Terminating due to SIGINT. Cleaning up shared memory.\n");
        shmdt(sharedClock);
        shmctl(shm_id, IPC_RMID, NULL);
        exit(0);
    }
}

void initProcessTable() {
    for (int i = 0; i < MAX_PROCS; i++) {
        procTable[i].active = 0;
        procTable[i].pid = 0;
        procTable[i].startSec = 0;
        procTable[i].startNano = 0;
    }
}

int getFreeSlot() {
    for (int i = 0; i < MAX_PROCS; i++) {
        if (procTable[i].active == 0) {
            return i;
        }
    }
    return -1;
}

void incrementClock(int nanoIncrement) {
    sharedClock[1] += nanoIncrement;
    if (sharedClock[1] >= 1000000000) {
        sharedClock[0] += 1;
        sharedClock[1] -= 1000000000;
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, handleSignal);

    int maxProcesses = 5, maxSimultaneous = 3, timeLimitForWorkers = 5, intervalMs = 100;
    int opt;
    while ((opt = getopt(argc, argv, "n:s:t:i:")) != -1) {
        switch (opt) {
            case 'n':
                maxProcesses = atoi(optarg);
                break;
            case 's':
                maxSimultaneous = atoi(optarg);
                break;
            case 't':
                timeLimitForWorkers = atoi(optarg);
                break;
            case 'i':
                intervalMs = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-n maxProcesses] [-s maxSimultaneous] [-t timeLimitForWorkers] [-i intervalMs]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    shm_id = shmget(SHMKEY, 2 * sizeof(int), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("oss: shmget");
        exit(EXIT_FAILURE);
    }

    sharedClock = (int *)shmat(shm_id, NULL, 0);
    if (sharedClock == (int *)-1) {
        perror("oss: shmat");
        exit(EXIT_FAILURE);
    }

    sharedClock[0] = 0;
    sharedClock[1] = 0;
    initProcessTable();

    int runningProcesses = 0, totalLaunched = 0;
    time_t startTime = time(NULL);

    while (totalLaunched < maxProcesses) {
        incrementClock(TIME_INCREMENT);

        int status;
        pid_t terminatedPid = waitpid(-1, &status, WNOHANG);
        if (terminatedPid > 0) {
            for (int i = 0; i < MAX_PROCS; i++) {
                if (procTable[i].pid == terminatedPid) {
                    procTable[i].active = 0;
                    printf("OSS: Process PID %d finished.\n", terminatedPid);
                    break;
                }
            }
        }

        if (runningProcesses < maxSimultaneous && totalLaunched < maxProcesses) {
            int slot = getFreeSlot();
            if (slot != -1) {
                pid_t childPid = fork();
                if (childPid == 0) {
                    char stayTime[10];
                    snprintf(stayTime, 10, "%d", rand() % timeLimitForWorkers + 1);
                    execl("./worker", "worker", stayTime, NULL);
                    perror("oss: execl");
                    exit(1);
                } else if (childPid > 0) {
                    procTable[slot].active = 1;
                    procTable[slot].pid = childPid;
                    procTable[slot].startSec = sharedClock[0];
                    procTable[slot].startNano = sharedClock[1];
                    runningProcesses++;
                    totalLaunched++;
                    printf("OSS: Launched process PID %d\n", childPid);
                } else {
                    perror("oss: fork");
                }
            }
        }

        if (sharedClock[1] % 500000000 == 0) {
            printf("OSS: Simulated time %ds %dns\n", sharedClock[0], sharedClock[1]);
            printf("Process Table:\n");
            for (int i = 0; i < MAX_PROCS; i++) {
                if (procTable[i].active) {
                    printf("Slot %d: Active: %d PID: %d StartSec: %d StartNano: %d\n", i, procTable[i].active, procTable[i].pid, procTable[i].startSec, procTable[i].startNano);
                }
            }
        }

        if (difftime(time(NULL), startTime) > 60) {
            printf("OSS: 60 seconds passed. Terminating workers.\n");
            for (int i = 0; i < MAX_PROCS; i++) {
                if (procTable[i].active) {
                    kill(procTable[i].pid, SIGTERM);
                }
            }
            break;
        }

        usleep(intervalMs * 1000);
    }

    shmdt(sharedClock);
    shmctl(shm_id, IPC_RMID, NULL);
    return 0;
}
