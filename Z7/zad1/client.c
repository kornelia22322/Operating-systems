#include "helper.h"

void intHandler(int);

void clear(void);

int take();

void process_cuts(int);


volatile int actualCuts = 0;

Fifo *fifo = NULL;
key_t key;

int shmID = -1;
int SID = -1;

void actualCutsHandler(int signo) {
    actualCuts++;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Clients creator: Specify clients Number and cuts Number!");
        exit(3);
    }

    int numberOfClients = (int) strtol(argv[1], '\0', 10);
    int numberOfCuts = (int) strtol(argv[2], '\0', 10);

    /* for keeping actuals cuts number - react on SIGRTMAX*/
    signal(SIGRTMAX, actualCutsHandler);
    atexit(clear);
    signal(SIGINT, intHandler);
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMAX);
    sigprocmask(SIG_BLOCK, &mask, NULL);


    /* fifo initialize */
    char *path = getenv("HOME");
    key = ftok(path, ID);
    if (key == -1) {
        printf("Getting shared memory key failed..");
        exit(3);
    }

    shmID = shmget(key, 0, 0);
    if (shmID == -1) {
        printf("Opening shared memory failed...");
        exit(3);
    }

    void *tmp = shmat(shmID, NULL, 0);
    fifo = (Fifo *) tmp;

    /* semaphores */

    SID = semget(key, 0, 0);

    for (int i = 0; i < numberOfClients; i++) {
        pid_t id = fork();
        if (id == -1) {
            printf("Fork failed...");
            exit(3);
        } else if (id == 0) {
            while (actualCuts < numberOfCuts) {

                struct sembuf sops;
                sops.sem_num = FINDOUT;
                sops.sem_op = -1;
                sops.sem_flg = 0;
                if (semop(SID, &sops, 1) == -1) {
                    printf("Taking FINDOUT semaphore failed...");
                    exit(3);
                }

                sops.sem_num = QUEUE;
                if (semop(SID, &sops, 1) == -1) {
                    printf("Taking QUEUE semaphore failed...");
                    exit(3);
                }

                int stat = take();

                sops.sem_op = 1; //release
                if (semop(SID, &sops, 1) == -1) {
                    printf("Releasing QUEUE semaphore failed...");
                    exit(3);
                }

                sops.sem_num = FINDOUT;
                if (semop(SID, &sops, 1) == -1) {
                    printf("Releasing FINDOUT semaphore failed!");
                    exit(3);
                }

                if (stat != -1) {
                    /* suspends the process
                   until delivery of a signal whose action is to invoke a signal handler
                   or to terminate a process. */
                    sigset_t fullMask;
                    sigsuspend(&fullMask);
                    printf("Time - %ld, %d got cut!\n", getTime(), getpid());
                    fflush(stdout);
                }
            }
            exit(0);
        }
    }

    printf("Clients created...\n");
    while (1) {
        wait(NULL);
    }

    return 0;
}

int take() {
    int stat = semctl(SID, 0, GETVAL);
    if (stat == -1) {
        printf("Getting value of BARBER semaphore failed!");
        exit(3);
    }

    pid_t myPID = getpid();

    if (stat == 0) {
        struct sembuf sops;
        sops.sem_num = BARBER;
        sops.sem_op = 1;
        sops.sem_flg = 0;

        if (semop(SID, &sops, 1) == -1) {
            printf("Awakening barber failed...");
            exit(3);
        }
        printf("Time - %ld, Client %d awakened the barber...\n", getTime(), myPID);
        fflush(stdout);
        if (semop(SID, &sops, 1) == -1) {
            printf("Awakening barber failed...");
            exit(3);
        }

        fifo->chair = myPID;

        return 1;
    } else {
        int res = push(fifo, myPID);
        if (res == -1) {
            printf("Time - %ld, Client %d can't find place....\n", getTime(), myPID);
            fflush(stdout);
            return -1;
        } else {
            printf("Time - %ld, Client %d landed in queue...\n", getTime(), myPID);
            fflush(stdout);
            return 0;
        }
    }
}

void clear(void) {
    if (shmdt(fifo) == -1) {
        printf("Detaching QUEUE shared memory failed...\n");
    }
    else {
        printf("Detached FIFO shared memory...\n");
    }
}

void intHandler(int signo) {
    exit(2);
}
