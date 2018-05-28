#include "helper.h"



#define FAILURE_EXIT(code, format, ...) { printf(format, ##__VA_ARGS__); exit(code);}

long getTime() {
    struct timespec time_spec;
    clock_gettime(CLOCK_MONOTONIC, &time_spec);
    return time_spec.tv_nsec;
}

void fifo_initialize(Fifo *fifo, int max_value) {
    fifo->max = max_value;
    fifo->head = -1;
    fifo->tail = 0;
    fifo->chair = 0;
}

int isEmptyFifo(Fifo *fifo) {
    if (fifo->head == -1) return 1;
    else return 0;
}

int isFullFifo(Fifo *fifo) {
    if (fifo->head == fifo->tail) return 1;
    else return 0;
}

pid_t pop(Fifo *fifo) {
    if (isEmptyFifo(fifo) == 1) return -1;

    fifo->chair = fifo->queue[fifo->head++];
    if (fifo->head == fifo->max) fifo->head = 0;

    if (fifo->head == fifo->tail) fifo->head = -1;

    return fifo->chair;
}

int push(Fifo *fifo, pid_t x) {
    if (isFullFifo(fifo) == 1) return -1;
    if (isEmptyFifo(fifo) == 1)
        fifo->head = fifo->tail = 0;

    fifo->queue[fifo->tail++] = x;
    if (fifo->tail == fifo->max) fifo->tail = 0;
    return 0;
}
