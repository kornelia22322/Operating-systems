#include "helper.h"
void clear(void);

void work();

void intHandler(int signum);


Fifo *fifo = NULL;
key_t key;
int shared_memory_id = -1;

int SID = -1;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Wrong chair number...\n");
        exit(2);
    }
    int numberOfChairs = (int) strtol(argv[1], '\0', 10);

    atexit(clear);
    signal(SIGINT, intHandler);

    /* creating FIFO struct */

    char *path = getenv("HOME");
    key = ftok(path, ID);
    /* if both IPC_CREAT and IPC_EXCL flags are set, shmget should fail if it is asked to create a segment for a key that already exists. */
    shared_memory_id = shmget(key, sizeof(Fifo), IPC_CREAT | IPC_EXCL | 0666);
    if (shared_memory_id == -1) {
        printf("Barber: creating shm failed!");
        exit(3);
    }

    void *tmp = shmat(shared_memory_id, NULL, 0);
    fifo = (Fifo *) tmp;

    fifo_initialize(fifo, numberOfChairs);

    /* creating semaphores */

    SID = semget(key, 3, IPC_CREAT | IPC_EXCL | 0666);

    union semun  {
        int val;
        struct semid_ds *buf;
        ushort *array;
    } arg;

    for (int i = 1; i < 3; i++) {
        arg.val = 1;
        semctl(SID, i, SETVAL, arg);
    }

    arg.val = 0;
    semctl(SID, 0, SETVAL, arg);

    work();

    return 0;
}

void work() {
    struct sembuf sops;
    sops.sem_flg = 0;
    while (1) {
        sops.sem_num = BARBER;
        sops.sem_op = -1;

        if (semop(SID, &sops, 1) == -1) {
            printf("BARBER up semaphore failed!");
            exit(3);
        }

        sops.sem_num = QUEUE;
        sops.sem_op = -1;
        semop(SID, &sops, 1);

        pid_t toCut = fifo->chair;

        sops.sem_op = 1;
        semop(SID, &sops, 1);

        printf("Time - %ld, Preparing to cuting... %d\n", getTime(), toCut);
        fflush(stdout);

        kill(toCut, SIGRTMAX); //send signal to client

        printf("Time - %ld, Finished cutting... %d\n", getTime(), toCut);
        fflush(stdout);

        while (1) {
            sops.sem_num = QUEUE;
            sops.sem_op = -1;
            if (semop(SID, &sops, 1) == -1) {
                printf("QUEUE up semaphore failed...\n");
                exit(3);
            }

            toCut = pop(fifo);

            if (toCut != -1) {
                sops.sem_op = 1;
                if (semop(SID, &sops, 1) == -1) {
                    printf("QUEUE release semaphore failed!");
                    exit(3);
                }

                /* cut operation */
                printf("Time - %ld, Preparing to cuting... %d\n", getTime(), toCut);
                fflush(stdout);

                kill(toCut, SIGRTMAX); //send signal to client

                printf("Time - %ld, Finished cutting... %d\n", getTime(), toCut);
                fflush(stdout);

            } else {
                //there aren't any clients in queue
                printf("Time - %ld, Barber is going to fall sleep...\n", getTime());
                fflush(stdout);
                sops.sem_num = BARBER;
                sops.sem_op = -1;
                if (semop(SID, &sops, 1) == -1) {
                    printf("BARBER UP semaphore failed!\n");
                };

                sops.sem_num = QUEUE;
                sops.sem_op = 1;
                if (semop(SID, &sops, 1) == -1) {
                    printf("QUEUE down sempaphore failed...");
                }
                break;
            }
        }
    }
}

void clear(void) {
    if (shmdt(fifo) == -1) printf("Detaching fifo shm failed..!\n");
    else printf("Detached queue shared memory!\n");

    if (shmctl(shared_memory_id, IPC_RMID, NULL) == -1) printf("Deleting fifo shared memory failed!\n");
    else printf("Deleted fifo shared memory...!\n");

    if (semctl(SID, 0, IPC_RMID) == -1) printf("Deleting semaphores failed...!");
    else printf("Deleted semaphores!\n");
}

void intHandler(int signo) {
    exit(2);
}
