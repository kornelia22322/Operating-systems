#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

const int number_of_arguments = 10;

int main(int argc, char **argv) {
    if(argc < 1) {
        printf("%s", "There's no enough arguments!");
        return 1;
    }
    char* filename = argv[1];
    FILE* file = fopen(filename,"r");
    if(file == NULL){
        perror(filename);
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    int counter = 0;
    char *parameters[number_of_arguments];
    int line_number = 0;

    /*  Pierwsze odwołanie do strtok() powinno posiadać s jako pierwszy argument.
    Następne wywołania powinny mieć jako pierwszy argument NULL.  */
    while ((read = getline(&line, &len, file)) != -1) {
        counter = 0;
        while((parameters[counter] = strtok(counter == 0 ? line : NULL, " \n\t")) != NULL){
            counter++;
        };
        pid_t pid = fork();
        if(pid == 0) {
            if(execvp(parameters[0], parameters) == -1){
                char buf[30];
                sprintf(buf,"Bład w wykonaniu lini: %d " , line_number);
                perror(buf);
                exit(EXIT_FAILURE);
            }
            exit(1);
        }
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
