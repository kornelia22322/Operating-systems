#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum) {
    union sigval a;
    sigqueue(getppid(), rand() % (SIGRTMAX - SIGRTMIN) + SIGRTMIN, a);
}

int main() {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigaction(SIGUSR1, &sa, NULL);

    srand((unsigned int) getpid());
    int sleepTime = (rand() % 10);

    printf("Child: Hello, my PID - %d, sleeping for %d\n", getpid(), sleepTime);
    sleep((unsigned int) sleepTime );
    kill(getppid(), SIGUSR1);
    return sleepTime;
}
