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

int privateClientQueueID = -1;
int publicClientQueueID = -1;

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

void remove(void) {
    if(msgctl(privateClientQueueID, IPC_RMID, NULL) > 0) {
        printf("Cleared..\n");
    }
}

int main(void) {
    if(atexit(remove) != 0) {
        printf("An error during remove queue\n");
        strerror(errno);
        exit(1);
    }
    signal(SIGINT, quit);

    key_t pubKey = SERVER_KEY;

    publicClientQueueID = msgget(pubKey, 0);
    key_t privKey = ftok(HOME, getpid());

    privateClientQueueID = msgget(privKey, IPC_CREAT | IPC_EXCL | 0666);
    msg loginMessage;
    loginMessage.mType = HELLO;
    loginMessage.receivedFrom = getpid();
    sprintf(loginMessage.messageContent, "%d", privKey);

    if(msgsnd(publicClientQueueID, &loginMessage, MESSAGE_SIZE, 0) < 0) {
        printf("Failed to login");
    }

    if(msgrcv(privateClientQueueID, &loginMessage, MESSAGE_SIZE, 0, 0) < 0) {
        printf("No server res");
    }

    if(!strcmp(loginMessage.messageContent, "maxclients")) {
        printf("Too many clients...\n");
    }


    msg message;
    char command[10];

    while (1) {
        message.receivedFrom = getpid();
        printf("MIRROR | CALC | TIME | END");


            message.receivedFrom = getpid();
        if (fgets(command, 256, stdin)) {
            strtok(command, "\n");
            if (strcmp(command, "MIRROR") == 0) {
                message.mType = MIRROR;
                if (fgets(message.messageContent, MAX_MSG_SIZE, stdin)) {
                    check_err(msgsnd(publicClientQueueID, &message, MESSAGE_SIZE, 0));
                    check_err(msgrcv(privateClientQueueID, &message, MESSAGE_SIZE, 0, 0));
                    printf("%s\n", message.messageContent);
                } else {
                    printf("%s", "size limit exceeded");
                }
            } else if (strcmp(command, "CALC") == 0) {
                message.mType = CALC;

                if (fgets(message.messageContent, MAX_MSG_SIZE, stdin)) {
                    check_err(msgsnd(publicClientQueueID, &message, MESSAGE_SIZE, 0));
                    check_err(msgrcv(privateClientQueueID, &message, MESSAGE_SIZE, 0, 0));
                    printf("%s\n", message.messageContent);
                }
            } else if (strcmp(command, "TIME") == 0) {
                message.mType = TIME;

                check_err(msgsnd(publicClientQueueID, &message, MESSAGE_SIZE, 0));
                check_err(msgrcv(privateClientQueueID, &message, MESSAGE_SIZE, 0, 0));
                printf("%s\n", message.messageContent);

            } else if (strcmp(command, "END") == 0) {
                message.mType = END;
                msgsnd(publicClientQueueID, &message, MESSAGE_SIZE, 0);
                remove();
                exit(1);
            } else printf("Malformed command!\n");
        }

    }
    return 0;
}
