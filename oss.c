#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>

#define SHMKEY 9876
#define MAX_PROCESSES 20
#define CLOCK_INCREMENT 1000000 // 1 millisecond in nanoseconds

struct PCB {
    int occupied;
    pid_t pid;
    int startSeconds;
    int startNano;
};

// Global variables
int shmid;
int *shmClock;
struct PCB processTable[MAX_PROCESSES];

void sig_handler(int signo) {
    if (signo == SIGINT) {
        printf("OSS: Received SIGINT. Cleaning up.\n");
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (processTable[i].occupied) {
                kill(processTable[i].pid, SIGTERM);
            }
        }
        shmdt(shmClock);
        shmctl(shmid, IPC_RMID, NULL);
        exit(0);
    }
}

void initProcessTable() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processTable[i].occupied = 0;
        processTable[i].pid = 0;
        processTable[i].startSeconds = 0;
        processTable[i].startNano = 0;
    }
}

int findAvailableSlot() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processTable[i].occupied == 0) {
            return i;
        }
    }
    return -1;
}

void incrementClock(int incrementNano) {
    shmClock[1] += incrementNano;
    if (shmClock[1] >= 1000000000) {
        shmClock[0]++;
        shmClock[1] -= 1000000000;
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sig_handler);

    int maxProcesses = 5;
    int maxSimultaneous = 3;
    int timeLimitForChildren = 5;
    int intervalInMsToLaunchChildren = 100;

    int opt;
    while ((opt = getopt(argc, argv, "n:s:t:i:")) != -1) {
        switch (opt) {
            case 'n': maxProcesses = atoi(optarg); break;
            case 's': maxSimultaneous = atoi(optarg); break;
            case 't': timeLimitForChildren = atoi(optarg); break;
            case 'i': intervalInMsToLaunchChildren = atoi(optarg); break;
            default:
                printf("Usage: %s [-n maxProcesses] [-s maxSimultaneous] [-t timeLimit] [-i interval]\n", argv[0]);
                exit(1);
        }
    }

    shmid = shmget(SHMKEY, 2 * sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("oss: shmget");
        exit(1);
    }

    shmClock = (int *)shmat(shmid, NULL, 0);
    if (shmClock == (int *)-1) {
        perror("oss: shmat");
        exit(1);
    }
    shmClock[0] = 0; shmClock[1] = 0;

    initProcessTable();

    int numProcesses = 0;
    int launchedProcesses = 0;
    int lastLaunchTimeS = shmClock[0];
    int lastLaunchTimeN = shmClock[1];

    while (launchedProcesses < maxProcesses || numProcesses > 0) {
        incrementClock(CLOCK_INCREMENT);

        int status;
        pid_t pid = waitpid(-1, &status, WNOHANG);
        if (pid > 0) {
            for (int i = 0; i < MAX_PROCESSES; i++) {
                if (processTable[i].pid == pid) {
                    processTable[i].occupied = 0;
                    numProcesses--;
                    printf("OSS: Process PID %d terminated.\n", pid);
                    break;
                }
            }
        }

        int timeSinceLastLaunch = (shmClock[0] - lastLaunchTimeS) * 1000 + 
                                  (shmClock[1] - lastLaunchTimeN) / 1000000;

        if (timeSinceLastLaunch >= intervalInMsToLaunchChildren &&
            numProcesses < maxSimultaneous &&
            launchedProcesses < maxProcesses) {
            int slot = findAvailableSlot();
            if (slot != -1) {
                pid_t childPid = fork();
                if (childPid == 0) {
                    usleep(500);
                    char timeToStay[10];
                    snprintf(timeToStay, 10, "%d", rand() % timeLimitForChildren + 1);
                    execl("./worker", "worker", timeToStay, NULL);
                    perror("oss: execl");
                    exit(1);
                } else if (childPid > 0) {
                    processTable[slot].occupied = 1;
                    processTable[slot].pid = childPid;
                    processTable[slot].startSeconds = shmClock[0];
                    processTable[slot].startNano = shmClock[1];
                    numProcesses++;
                    launchedProcesses++;
                    lastLaunchTimeS = shmClock[0];
                    lastLaunchTimeN = shmClock[1];
                    printf("OSS: Launched worker process PID %d\n", childPid);
                } else {
                    perror("oss: fork");
                }
            }
        }
    }

    printf("OSS: All workers complete. Cleaning up shared memory.\n");
    shmdt(shmClock);
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("oss: Failed to remove shared memory");
    } else {
        printf("OSS: Shared memory successfully removed.\n");
    }

    return 0;
}
