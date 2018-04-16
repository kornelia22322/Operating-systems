#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

pid_t pid = 0;


int main(int argc, char** argv) {

    pid = fork();


    if (pid == 0){
        //desctiption of execl:
        //The first argument, by convention, should point to the filename associated with the file being executed.
        execl("./script.sh", "./script.sh", NULL);
        exit(EXIT_SUCCESS);
    }

}
