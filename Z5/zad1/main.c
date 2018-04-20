#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <string.h>

const int N = 20; //ilość połączonych komend

void execute_single_line(char * line){
    char *program_line[N];
    char *args[N][6];
    pid_t pids[N];
    program_line[0] = strtok(line,"|\n");
    int program_len = 1;

    // To signal strtok() that you want to keep searching the same string, you pass a NULL pointer as its first argument.
    while(program_len < 20 && (program_line[program_len] = strtok(NULL,"|\n")))
    {
        program_len++;
    }

    for(int i = 0; i< program_len; i++){
        args[i][0]= strtok(program_line[i]," ");
        if(args[i][0] != NULL) {
            int j = 1;
            do {
                args[i][j] = strtok(NULL, " \n");
                if (args[i][j] == NULL) {
                    break;
                }
                j++;
            } while (j < 5);
            args[i][j] = NULL;
        }
    }

    int fd[program_len][2];
    for(int i=0; i<program_len; i++){
        pipe(fd[i]);
    }

    for(int i=0; i<program_len; i++){
        int new_process=fork();

        //printf("%d %d\n", new_process, i);
        if(new_process<0){
            printf("Fork failed\n");
            exit(1);
        }

        else if(new_process==0){
            if(i!=program_len-1){ //if it's not the last one you have to redirect the output
                close(fd[i][0]);
                dup2(fd[i][1], STDOUT_FILENO);
            }
            if(i>0) {
                dup2(fd[i-1][0], STDIN_FILENO);
                close(fd[i-1][1]);
            }

            int new_exec;
            // printf("Exec %d, %s %s file with descriptord %d %d %d %d\n", i, args[i][0], args[i][1], fd[i][0], fd[i][1], fd[i-1][0], fd[i-1][1]);
            new_exec=execvp(args[i][0], args[i]);
            if(new_exec==-1){
                exit(1);
            }
        } else { //parent process
            if(i<program_len-1)
                close(fd[i][1]);  //closing the unused write end - last needs to print to stdout the result

            pids[i] = new_process;
        }
    }
    for(int i=0; i< program_len-1; i++){
        close(fd[i][0]);
        close(fd[i][1]);
    }

    int status;
    for(int i = 0; i < program_len; i++){
        if (waitpid(pids[i], &status, 0) == -1) {
            perror("failed to wait for child");
            exit(1);
        }
    }


}

int main(int argc, char *argv[])
{
    if(argc < 2){
        printf("Specify the file..");
        exit(1);
    }

    FILE* file= fopen(argv[1], "r");

    char *buffer;
    size_t len = 0;
    fflush(stdout);
    int line_number = 0;
    while (getline(&buffer, &len,file) != -1)
    {
        printf("LINE NUMBER: %d\n", line_number);
        execute_single_line(buffer);
        line_number++;
    }

    return 0;
}
