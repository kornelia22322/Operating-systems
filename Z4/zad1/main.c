#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int await = 0;
int is_dead = 1;

void stop_signal_toggle(int sig) {
    if(await == 0) {
        printf("\nOdebrano sygnał SIGTSTP. Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
    }
    if(await) {
        await = 0;
    } else {
        await = 1;
    }
}

void sigint_signal(int sig) {
    printf("\nOdebrano sygnał SIGINT");
    exit(0);
}


int main(int argc, char** argv) {

    struct sigaction sa;
    sa.sa_handler = stop_signal_toggle;
    pid_t pid;

    while(1){
        sigaction(SIGTSTP, &sa, NULL);
        signal(SIGINT, sigint_signal);

        if(await == 0) {
            if(is_dead){ //if it is not alive raise it
                is_dead = 0;
                pid = fork();
                if (pid == 0){
                    //desctiption of execl:
                    //The first argument, by convention, should point to the filename associated with the file being executed.
                    execl("./script.sh", "./script.sh", NULL);
                    exit(0);
                }
            }
        } else {
            if (is_dead == 0) { //if it is alive kill it
                kill(pid, SIGKILL);
                is_dead = 1;
            }
        }
    }

}
