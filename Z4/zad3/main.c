#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

int L;
int TYPE;
sig_atomic_t sended_signals = 0;
sig_atomic_t received_by_child = 0;
sig_atomic_t received_in_parent = 0;
volatile pid_t child;

void mother_handler(int signum, siginfo_t *info, void *context);
void child_handler(int signum, siginfo_t *info, void *context);

void mother_fun() {
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = mother_handler;

    sleep(1);

    sigaction(SIGINT, &act, NULL);
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGRTMIN, &act, NULL);

    if (TYPE == 1 || TYPE == 2) {
        int i;
        for (i = 0; i < L; i++) {
            printf("Mother: Sending SIGUSR1\n");
            kill(child, SIGUSR1);
            if (TYPE == 2) {
                sigset_t mask;
                sigfillset(&mask);

                //The sigdelset() function deletes the individual signal specified by signo
                // from the signal set pointed to by set. Applications should call either sigemptyset() or sigfillset()
                // at least once for each object of type sigset_t prior to any other use of that object.
                sigdelset(&mask, SIGUSR1);
                sigdelset(&mask, SIGINT);
                sigsuspend(&mask);
            }
            sended_signals++;
        }
        printf("Mother: Sending SIGUSR2\n");
        kill(child, SIGUSR2);
    }

    if (TYPE == 3) {
        int i;
        for (i = 0;  i < L;  i++) {
            printf("Mother: Sending SIGRTMIN\n");
            kill(child, SIGRTMIN);
            sended_signals++;
        }
        sended_signals++;
        printf("Mother: Sending SIGRTMIN+1\n");
        kill(child, SIGRTMIN+1);
    }

    wait(NULL);
}

void child_fun() {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = child_handler;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGRTMIN, &sa, NULL);
    sigaction(SIGRTMIN+1, &sa, NULL);

    while (1) {
        sleep(1);
    }
}

void mother_handler(int signum, siginfo_t *info, void *context) {
    if (signum == SIGINT) {
        printf("Mother: Received SIGINT\n");
        kill(child, SIGUSR2);
        printf("Signals sent: %d\n",  sended_signals);
        printf("Signals received from child: %d\n", received_in_parent);
        exit(0);
    }
    if (info->si_pid != child) return;

    if ((TYPE == 1 || TYPE == 2)) {
        if(signum == SIGUSR1){
            received_in_parent++;
            printf("Mother: Received SIGUSR1 form Child\n");
        }
    }

    if (TYPE == 3) {
        if(signum == SIGRTMIN) {
            received_in_parent++;
            printf("Mother: Received SIGRTMIN from Child\n");
        }
    }
}

void child_handler(int signum, siginfo_t *info, void *context) {
    if (signum == SIGINT) { //block all signals afterwards
        sigset_t mask;
        //sigfillset() initializes set to full, including all signals.
        sigfillset(&mask);
        sigprocmask(SIG_SETMASK, &mask, NULL);
        printf("Signals received by child: %d\n",  received_by_child);
        exit(0);
    }

    if (info->si_pid != getppid()) return;

    if (TYPE == 1 || TYPE == 2) {
        if (signum == SIGUSR1) {
            received_by_child++;
            kill(getppid(), SIGUSR1);
            //printf("SIGUSR1 received by child - sending back\n")
        } else if (signum == SIGUSR2) {
            //printf("SIGUSR2 received by child - terminating\n")
            printf("Signals received by child: %d\n",  received_by_child);
            exit(0);
        }
    }

    if (TYPE == 3) {
        if (signum == SIGRTMIN) {
             received_by_child++;
            kill(getppid(), SIGRTMIN);
            printf("Child: SIGRTMIN received and sent back\n");
        } else if (signum == SIGRTMIN+1) {
             received_by_child++;
            printf("Child: SIGRTMIN+1 received - terminating\n");
            printf("Signals received by child: %d\n", received_by_child);
            exit(0);
        }
    }
}


int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Missing some parameters");
        exit(1);
    }

    L = (int) strtol(argv[1], '\0', 10);
    TYPE = (int) strtol(argv[2], '\0', 10);

    child = fork();
    if (!child) child_fun();
    else mother_fun();
    printf("Signals sent: %d\n", sended_signals);
    printf("Signals received from child: %d\n", received_in_parent);
    return 0;
}
