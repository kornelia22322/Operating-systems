#include "helper.h"

#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>


void clear(void);

void work();

void intHandler(int signum);

const char *shmPath = "/shm";
const char *barberPath = "/barber";
const char *queuePath = "/fifo";
const char *findoutPath = "/findout";

Fifo *fifo = NULL;
sem_t *BARBER;
sem_t *QUEUE;
sem_t *FINDOUT;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Wrong chair number...\n");
        exit(2);
    }
    int numberOfChairs = (int) strtol(argv[1], '\0', 10);

    atexit(clear);
    signal(SIGINT, intHandler);

    /* creating FIFO struct */

    int shared_memory_id = shm_open(shmPath, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (shared_memory_id == -1) {
        printf("Barber: creating shm failed!");
        exit(3);
    }

    if(ftruncate(shared_memory_id, sizeof(Fifo)) == -1) {
        printf("Something went wrong with attach memory...");
        exit(3);
    }
    void *tmp = mmap(NULL, sizeof(Fifo), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_id, 0);
    fifo = (Fifo *) tmp;

    fifo_initialize(fifo, numberOfChairs);

    /* creating semaphores */


    BARBER = sem_open(barberPath, O_CREAT | O_EXCL | O_RDWR, 0666, 0);
    if (BARBER == SEM_FAILED) {
        printf("Barber: creating BARBER semaphore failed!");
        exit(3);
    };

    QUEUE = sem_open(queuePath, O_CREAT | O_EXCL | O_RDWR, 0666, 1);
    if (QUEUE == SEM_FAILED) {
        printf("Barber: creating FIFO semaphore failed!");
        exit(3);
    }

    FINDOUT = sem_open(findoutPath, O_CREAT | O_EXCL | O_RDWR, 0666, 1);
    if (FINDOUT == SEM_FAILED) {
        printf("Barber: creating CHECKER semaphore failed!");
        exit(3);
    }

    work();
    return 0;
}

void work() {
    while (1) {
        if (sem_wait(BARBER) == -1) {
            printf("Decrement barber semaphore failed...");
            exit(3);
        };
        if (sem_post(BARBER) == -1){
            printf("Increment barber semaphore failed...");
        }

        sem_wait(QUEUE);
        pid_t toCut = fifo->chair;
        sem_post(QUEUE);
        printf("Time - %ld, Preparing to cut... %d\n", getTime(), toCut);
        fflush(stdout);
        kill(toCut, SIGRTMAX);
        printf("Time: %ld, Finished cutting %d\n", getTime(), toCut);
        fflush(stdout);

        while (1) {
            sem_wait(QUEUE);
            toCut = pop(fifo);

            if (toCut != -1) {
                sem_post(QUEUE);
                printf("Time - %ld, Preparing to cut... %d\n", getTime(), toCut);
                fflush(stdout);
                kill(toCut, SIGRTMAX);
                printf("Time: %ld, Finished cutting %d\n", getTime(), toCut);
                fflush(stdout);
            } else {
                printf("Time: %ld, Barber falling asleep...\n", getTime());
                fflush(stdout);

                sem_wait(BARBER);
                sem_post(QUEUE);
                break;
            }
        }
    }
}

void clear(void) {
    if (munmap(fifo, sizeof(fifo)) == -1) printf("Detaching QUEUE shared memory failed...!\n");
    else printf("Detached queue shared memory...\n");

    if (shm_unlink(shmPath) == -1) printf("Deleting QUEUE shared memory failed...!\n");
    else printf("Deleted queue shared memory...\n");

    sem_close(BARBER);
    sem_unlink(barberPath);
    sem_close(QUEUE);
    sem_unlink(queuePath);
    sem_close(FINDOUT);
    sem_unlink(findoutPath);
}

void intHandler(int signo) {
    exit(2);
}
