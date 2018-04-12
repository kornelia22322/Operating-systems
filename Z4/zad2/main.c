#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

int N;
int K;

sig_atomic_t n; //how many children do we have
sig_atomic_t k;
volatile pid_t *children_array;
volatile pid_t *array_for_waiting_pid;

void SIGINT_handler(int signum, siginfo_t *info, void *context) {
    printf("\rMother - Received SIGINT\n");
    for (int i = 0; i < N; i++) {
        kill(children_array[i], SIGKILL);
        waitpid(children_array[i], NULL, 0);
    }
    exit(0);
}

void RT_SIGNAL_handler(int signum, siginfo_t *info, void *context) {
    //only printf
    printf("Received SIGRT: SIGMIN+%i, from PID: %d\n", signum - SIGRTMIN, info->si_pid);
}


void SIGUSR1_handler(int signum, siginfo_t *info, void *context) {
    printf("Received SIGUSR1 form PID: %d\n", info->si_pid);

    if (k >= K) {
        printf("Mother - Sending SIGUSR1 to child PID: %d\n", info->si_pid);
        kill(info->si_pid, SIGUSR1);
        waitpid(info->si_pid, NULL, 0);
    } else {
        array_for_waiting_pid[k] = info->si_pid;
        k++;
        if (k == K) {
            for (int i = 0; i < K; i++) {
                printf("Mother - Sending SIGUSR1 to child PID: %d\n", array_for_waiting_pid[i]);
                kill(array_for_waiting_pid[i], SIGUSR1);
                waitpid(array_for_waiting_pid[i], NULL, 0);
            }
        }
    }
}

//When a child process stops or terminates, SIGCHLD is sent to the parent process.
void SIGCHLD_handler(int signum, siginfo_t *info, void *context) {
    if (info->si_code == CLD_EXITED) {
        printf("Child %d terminated with exit status: %d\n", info->si_pid, info->si_status);
    } else {
        printf("Child %d has terminated by signal: %d\n", info->si_pid, info->si_status);
    }
    n--;
    if (n == 0) {
        printf("Terminating - no more children\n");
        exit(0);
    }
    for (int i = 0; i < N; i++) {
        if (children_array[i] == info->si_status) {
            children_array[i] = -1;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Missing some parameters");
        exit(1);
    }

    N = (int) strtol(argv[1], '\0', 10);
    K = (int) strtol(argv[2], '\0', 10);

    if(K > N) {
        printf("N-value needs to be larger than K-value!\n");
        exit(1);
    }

    children_array = (pid_t*) calloc((size_t) N, sizeof(pid_t));
    array_for_waiting_pid = (pid_t*) calloc((size_t) N, sizeof(pid_t));
    n = 0;

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;

    //SIGINT_handler
    sa.sa_sigaction = SIGINT_handler;
    sigaction(SIGINT, &sa, NULL);

    //SIGUSR1
    sa.sa_sigaction = SIGUSR1_handler;
    sigaction(SIGUSR1, &sa, NULL);

    sa.sa_sigaction = SIGCHLD_handler;
    sigaction(SIGCHLD, &sa, NULL);

    for (int i = SIGRTMIN; i <= SIGRTMAX; i++) {
        sa.sa_sigaction = RT_SIGNAL_handler;
        sigaction(i, &sa, NULL);
    }

    for (int i = 0; i < N; i++) {
        pid_t pid = fork();
        if (!pid) {
            execl("./child", "./child", NULL);
            exit(0);
        } else {
            children_array[i] = pid;
            n++;
        }
    }

    //supposed to wait until all child process's are completes first
    //If the current process have no child processes, wait(NULL) returns -1.
    //Otherwise it waits until one of them exits, and returns it's process ID.
    while(wait(NULL));
}
