#include "helper.h"

void intHandler(int);

void clear(void);

int take();

void process_cuts(int);


volatile int actualCuts = 0;

const char *shmPath = "/shm";
const char *barberPath = "/barber";
const char *queuePath = "/fifo";
const char *findoutPath = "/findout";

Fifo *fifo = NULL;
sem_t *BARBER;
sem_t *QUEUE;
sem_t *FINDOUT;

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

    int shared_memory_id = shm_open(shmPath, O_RDWR, 0666);
    if (shared_memory_id  == -1) {
        printf("Clients creator: opening shared memory failed!");
        exit(3);
    }

    void *tmp = mmap(NULL, sizeof(Fifo), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_id, 0);
    if (tmp == (void *) (-1)) {
        printf("Clients creator: attaching shm failed!");
        exit(3);
    }
    fifo = (Fifo *) tmp;

    /* semaphores */


    BARBER = sem_open(barberPath, O_RDWR);
    if (BARBER == SEM_FAILED) {
        printf("Barber: creating BARBER semaphore failed!");
        exit(3);
    };

    QUEUE = sem_open(queuePath, O_RDWR);
    if (QUEUE == SEM_FAILED) {
        printf("Barber: creating FIFO semaphore failed!");
        exit(3);
    }

    FINDOUT = sem_open(findoutPath, O_RDWR);
    if (FINDOUT == SEM_FAILED) {
        printf("Barber: creating CHECKER semaphore failed!");
        exit(3);
    }

    for (int i = 0; i < numberOfClients; i++) {
        pid_t id = fork();
        if (id == -1) {
            printf("Fork failed...");
            exit(3);
        } else if (id == 0) {
            while (actualCuts < numberOfCuts) {
                sem_wait(FINDOUT);
                sem_wait(QUEUE);
                int stat = take();
                sem_post(QUEUE);
                sem_post(FINDOUT);
                if (stat != -1) {
                     sigset_t fullMask;
                    sigsuspend(&fullMask);
                    printf("Time - %ld, %d just got cut...\n", getTime(), getpid());
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
    int stat;
    sem_getvalue(BARBER, &stat);
    pid_t myPID = getpid();
    if (stat == 0) {
        sem_post(BARBER); //awake barber
        printf("Time: %ld, %d has awakened barber!\n", getTime(), myPID);
        fflush(stdout);
        fifo->chair = myPID;
        return 1;
    } else {
        int stat0 = push(fifo, myPID);
        if (stat0 == -1) {
            printf("Time: %ld, %d can't find free place...\n", getTime(), myPID);
            fflush(stdout);
            return -1;
        } else {
            printf("Time: %ld, %d landed in the queue!\n", getTime(), myPID);
            fflush(stdout);
            return 0;
        }
    }
}

void clear(void) {
    if (munmap(fifo, sizeof(fifo)) == -1) printf("Detaching queue shared memory failed...");
    else printf("Detached fifo shared memory...\n");

    sem_close(BARBER);
    sem_close(QUEUE);
    sem_close(FINDOUT);
}

void intHandler(int signo) {
    exit(2);
}
