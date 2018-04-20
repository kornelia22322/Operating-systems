#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <string.h>

#define date_size 33

int main(int argc, char**argv){
    if(argc<3){
        printf("Wrong arguments given to the program, please try agen\n");
        exit(EXIT_FAILURE);
    }
    int fifoptr=open(argv[1],O_WRONLY);

    //how many times display the date
    int N=(int)strtol(argv[2],NULL,10);

    srand((unsigned int)getpid()%100+1);
    printf("Child: My pid is: %d\n",getpid());

    int i;
    char date_buffer[date_size];
    char pipe_output[PIPE_BUF];

    for(i=0;i<N;i++){
        //create new pipe -
        FILE*date_pipe=popen("date","r");

        if(date_pipe==NULL){
            perror("Something wrong with getting date");
            exit(EXIT_FAILURE);
        }

        fgets(date_buffer,date_size,date_pipe);
        fclose(date_pipe);

        sprintf(pipe_output,"My pid is %d and my time is %s\n",getpid(),date_buffer);
        write(fifoptr,pipe_output,strlen(pipe_output));
        sleep((unsigned int) (rand() % 3 + 1));
    }
    close(fifoptr);
    return 0;
}
