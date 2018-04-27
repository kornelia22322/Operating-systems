#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <signal.h>

#include "common.h"

#include <mqueue.h>

int privateQueueID = -1;
int publicQueueID = -1;

void check_err(int x) {
    if(x<0){
        printf("An error occured\n");
        strerror(errno);
        exit(1);
    }
}

void quit(int signo) {
    exit(1);
}

void rem(void) {
    mq_close(publicQueueID);
    mq_close(privateQueueID);
    mq_unlink(NAME);
}

int main(void) {
    if(atexit(rem) != 0) {
        printf("An error during remove queue\n");
        strerror(errno);
        exit(1);
    }
    signal(SIGINT, quit);

    char path[256];
    int pid = getpid();

    struct mq_attr parameters;
    parameters.mq_msgsize = MESSAGE_SIZE;
    parameters.mq_maxmsg = 20;


    sprintf(path, "/%d", pid);


    msg loginMessage;
    loginMessage.mType = HELLO;
    loginMessage.receivedFrom = getpid();


    publicQueueID = mq_open(NAME, O_WRONLY);
    privateQueueID = mq_open(path, O_RDONLY | O_CREAT | O_EXCL, 0777, &parameters);


    if(mq_send(publicQueueID, (char *) &loginMessage, MESSAGE_SIZE, 1) < 0) {
        printf("Failed to login");
    }

    if(mq_receive(privateQueueID, (char *) &loginMessage, MESSAGE_SIZE, NULL) < 0) {
        printf("No server");
    }

    if(!strcmp(loginMessage.messageContent, "maxclients")) {
        printf("Too many clients...\n");
    }


    msg message;
    char command[10];

    while (1) {

        message.receivedFrom = getpid();

        printf("MIRROR | CALC | TIME | END");


        if (fgets(command, 256, stdin)) {
            strtok(command, "\n");
            if (strcmp(command, "MIRROR") == 0) {
                message.mType = MIRROR;
                if (fgets(message.messageContent, MAX_MSG_SIZE, stdin)) {
                    check_err(mq_send(publicQueueID, (char *) &message, MESSAGE_SIZE, 1));
                    check_err(mq_receive(privateQueueID, (char *) &message, MESSAGE_SIZE, NULL));
                    printf("%s\n", message.messageContent);
                }
            } else if (strcmp(command, "CALC") == 0) {
                message.mType = CALC;

                if (fgets(message.messageContent, MAX_MSG_SIZE, stdin)) {

                    check_err(mq_send(publicQueueID, (char *) &message, MESSAGE_SIZE, 1));
                    check_err(mq_receive(privateQueueID, (char *) &message, MESSAGE_SIZE, NULL));
                    printf("%s\n", message.messageContent);
                }
            } else if (strcmp(command, "TIME") == 0) {
                message.mType = TIME;


                    check_err(mq_send(publicQueueID, (char *) &message, MESSAGE_SIZE, 1));
                    check_err(mq_receive(privateQueueID, (char *) &message, MESSAGE_SIZE, NULL));
                printf("%s\n", message.messageContent);

            } else if (strcmp(command, "END") == 0) {
                message.mType = END;

                check_err(mq_send(publicQueueID, (char *) &message, MESSAGE_SIZE, 1));
                rem();
                exit(1);
            } else printf("Malformed command!\n");
        }

    }
    return 0;
}
