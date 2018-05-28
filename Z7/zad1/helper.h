#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define ID 18

typedef enum semType {
    BARBER = 0, QUEUE = 1, FINDOUT = 2
} semType;

typedef struct Fifo {
    int max;
    pid_t chair;
    pid_t queue[512];
    int head;
    int tail;
} Fifo;

void fifo_initialize(Fifo *fifo, int max_value);

pid_t pop(Fifo *fifo);

int push(Fifo *fifo, pid_t pid);

long getTime();
