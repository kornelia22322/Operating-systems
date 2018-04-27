#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <mqueue.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "common.h"



void waitForMessages(struct msg *receivedMessage);

msg createNewMessage(int receivedFrom, int *senderID);

int searchPID(int i);

void removePID(int i);
char * reverse(char *str, int len);

int publicQueueID = -1;

int clientPID[MAX_CLIENTS];
int clientQUEUE[MAX_CLIENTS];
int howManyClients = 0;

void check_err(int x) {
    if(x<0){
        printf("An error occured\n");
        strerror(errno);
        exit(1);
    }
}

void rmQueue(void) {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        mq_close(clientQUEUE[i]);
    }
    mq_close(publicQueueID);
    mq_unlink(NAME);
}

void quit(int signo) {
    rmQueue();
    exit(0);
}

int main() {
    if(atexit(rmQueue) != 0) {
        printf("An error during remove queue\n");
        strerror(errno);
        exit(1);
    }

    signal(SIGINT, quit);

    struct mq_attr parameters;
    parameters.mq_msgsize = MESSAGE_SIZE;
    parameters.mq_maxmsg = 8;

    publicQueueID = mq_open(NAME, O_RDONLY | O_CREAT | O_EXCL, 0777, &parameters);
    msg readMessage;

    while (1) {
        mq_receive(publicQueueID, (char *) &readMessage, MESSAGE_SIZE, NULL);
        waitForMessages(&readMessage);
    }
    return 0;
}

void waitForMessages(struct msg *receivedMessage) {

    if (receivedMessage == NULL) return;


    printf("GET request of type %ld from %d\n", receivedMessage->mType, receivedMessage->receivedFrom);
    fflush(stdout);

    switch (receivedMessage->mType) {
        case HELLO: {
            char path[32];
            sprintf(path, "/%d", receivedMessage->receivedFrom);

            int clientQueue = mq_open(path, O_WRONLY);

            check_err(clientQueue);
            msg returnMessage;
            int senderPID = receivedMessage->receivedFrom;
            returnMessage.mType = CLIENTSETUP;
            returnMessage.receivedFrom = getpid();

            if (howManyClients >= MAX_CLIENTS) {
                return;
            }


            sprintf(returnMessage.messageContent, "ok");

            clientPID[howManyClients] = senderPID;
            clientQUEUE[howManyClients] = clientQueue;
            howManyClients++;

            mq_send(clientQueue, (char *) &returnMessage, MESSAGE_SIZE, 1);
            break;
        }
        case MIRROR: {
            int sender = -1;
            msg message;

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clientPID[i] == receivedMessage->receivedFrom) {
                    sender = clientQUEUE[i];
                }
            }
            if(sender == -1) {
                printf("Don't have such a client..\n");
            };
            message.receivedFrom = getpid();

            strcpy(message.messageContent, receivedMessage->messageContent);
            strtok(message.messageContent, "\n");
            reverse(message.messageContent, strlen(message.messageContent));
            printf("%s\n", message.messageContent);

            mq_send(sender, (char *) &message, MESSAGE_SIZE, 1);
            break;
        }

        case TIME: {
            time_t rawtime;
            struct tm *timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);

            msg returnMessage;
            int senderID = receivedMessage->receivedFrom;
            returnMessage.mType = TIME;
            returnMessage.receivedFrom = getpid();

            strcpy(returnMessage.messageContent, asctime(timeinfo));
            mq_send(senderID, (char *) &returnMessage, MESSAGE_SIZE, 1);
            break;
        }
        case CALC: {
            int res;

            msg returnMessage;
            int senderID = receivedMessage->receivedFrom;
            returnMessage.mType = CLIENTSETUP;
            returnMessage.receivedFrom = getpid();

            char operands[] = {'+', '-', '/', '*'};

            for(int i = 0; i < 4; i++) {
                if(strchr(receivedMessage->messageContent, operands[i]) != NULL) {
                    if(i == 0) {
                        char* x = strtok(receivedMessage->messageContent, "+");
                        char* y = strtok(NULL, "+");
                        res = atoi(x) + atoi(y);
                    } else if(i == 1) {
                        char* x = strtok(receivedMessage->messageContent, "-");
                        char* y = strtok(NULL, "-");
                        res = atoi(x) - atoi(y);
                    } else if(i == 3) {
                        char* x = strtok(receivedMessage->messageContent, "*");
                        char* y = strtok(NULL, "*");
                        res = atoi(x) * atoi(y);
                    } else if(i == 4) {
                        char* x = strtok(receivedMessage->messageContent, "//");
                        char* y = strtok(NULL, "//");
                        res = atoi(x) / atoi(y);
                    }
                }
            }


            sprintf(returnMessage.messageContent, "%d", res);
            mq_send(senderID, (char *) &returnMessage, MESSAGE_SIZE, 1);
            break;
        }
        case END: {
            //remove client from array and shift
            int index = searchPID(receivedMessage->receivedFrom);
            removePID(index);
            break;
        }
        default:
            break;
    }
}

int searchPID(int pid){
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clientPID[i] == pid) {
            return i;
        }
    }
    return -1;
}

void removePID(int i) {
    for(int j = i; j < howManyClients - 1; j++) {
        clientPID[j] = clientPID[j+1];
        clientQUEUE[j] = clientQUEUE[j+1];
    }
    clientPID[howManyClients - 1] = 0;
    clientQUEUE[howManyClients - 1] = 0;
    howManyClients--;
}

char * reverse(char *str, int len) {
    char *p1 = str;
    char *p2 = str + len - 1;

    while (p1 < p2) {
        char tmp = *p1;
        *p1++ = *p2;
        *p2-- = tmp;
    }

    return str;
}
