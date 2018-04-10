#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

const int number_of_arguments = 10;

int main(int argc, char **argv) {
    if(argc < 1) {
        printf("%s", "There's no enough arguments! - specify file");
        return 1;
    }
    char* filename = argv[1];
    FILE* file = fopen(filename,"r");
    if(file == NULL){
        perror(filename);
        return 1;
    }

    struct rlimit mem;
    struct rlimit cpu;

    if(argc < 4){
        printf("Brak 2 i 3 argumentu(ograniczenia)!\n");
        return 1;
    }

    long int time_limit = strtol(argv[2], NULL, 10);
    cpu.rlim_max = (rlim_t) time_limit;
    cpu.rlim_cur = (rlim_t) time_limit;

    long int memory_limit = strtol(argv[3], NULL, 10) * 1024 * 1024;
    mem.rlim_max = (rlim_t) memory_limit;
    mem.rlim_cur = (rlim_t) memory_limit;


    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    int counter = 0;
    char *parameters[number_of_arguments];
    int line_number = 0;

    struct rusage prev_usage;
    getrusage(RUSAGE_CHILDREN, &prev_usage);

    /*  Pierwsze odwołanie do strtok() powinno posiadać s jako pierwszy argument.
    Następne wywołania powinny mieć jako pierwszy argument NULL.  */
    while ((read = getline(&line, &len, file)) != -1) {
        counter = 0;
        while((parameters[counter] = strtok(counter == 0 ? line : NULL, " \n\t")) != NULL){
            counter++;
        };
        pid_t pid = fork();
        if(pid == 0) {
            if (setrlimit(RLIMIT_CPU, &cpu) != 0) {
                printf("Cannot set this limit cpu");
                return 1;
            }

            if (setrlimit(RLIMIT_AS, &mem) != 0) {
                printf("Cannot set this limit memory");
                return 1;
            }

            if(execvp(parameters[0], parameters) == -1){
                char buf[30];
                sprintf(buf,"Bład w wykonaniu lini: %d " , line_number);
                perror(buf);
                exit(EXIT_FAILURE);
            }
            exit(1);
        }

        struct rusage usage;
        getrusage(RUSAGE_CHILDREN, &usage);
        struct timeval ru_utime;
        struct timeval ru_stime;
        timersub(&usage.ru_utime, &prev_usage.ru_utime, &ru_utime);
        timersub(&usage.ru_stime, &prev_usage.ru_stime, &ru_stime);
        prev_usage = usage;

        printf("\n");
        printf("User CPU time used: %d.%d seconds,  system CPU time used: %d.%d seconds\n\n", (int) ru_utime.tv_sec,
               (int) ru_utime.tv_usec, (int) ru_stime.tv_sec, (int) ru_stime.tv_usec);

        int status;
        wait(&status);
        if (status) {
           printf( "Error while executing");
           return 1;
        }
        line_number++;

    }

    free(line);
    fclose(file);
    return 0;
}
