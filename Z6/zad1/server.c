#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
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
        if(msgctl(clientQUEUE[i], IPC_RMID, NULL) > 0) {
            printf("Cleared..\n");
        }
    }
    check_err(publicQueueID);
    if(msgctl(publicQueueID, IPC_RMID, NULL) > 0) {
        printf("Cleared..\n");
    }
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

    publicQueueID = msgget(SERVER_KEY, IPC_EXCL | IPC_CREAT | 0777);
    if(publicQueueID < 0) {
        printf("An error during create queue\n");
        strerror(errno);
        exit(1);
    }

    msg readMessage;

    while (true) {
        msgrcv(publicQueueID, &readMessage, MESSAGE_SIZE, 0, 0);
        if (errno) exit(0);
        waitForMessages(&readMessage);
    }
    return 0;
}

void waitForMessages(struct msg *receivedMessage) {
    printf("GET request of type %ld from %d\n", receivedMessage->mType, receivedMessage->receivedFrom);
    fflush(stdout);

    switch (receivedMessage->mType) {
        case HELLO: {
            key_t queueKey;
            check_err(sscanf(receivedMessage->messageContent, "%d", &queueKey));
            int queueID = msgget(queueKey, 0);

            check_err(queueID);
            msg returnMessage;
            int senderPID = receivedMessage->receivedFrom;
            returnMessage.mType = CLIENTSETUP;
            returnMessage.receivedFrom = getpid();

            if (howManyClients >= MAX_CLIENTS) {
                return;
            }


            sprintf(returnMessage.messageContent, "ok");

            clientPID[howManyClients] = senderPID;
            clientQUEUE[howManyClients] = queueID;
            howManyClients++;

            check_err(msgsnd(queueID, &returnMessage, MESSAGE_SIZE, 0));
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

            check_err(msgsnd(sender, &message, MESSAGE_SIZE, 0));
            break;
        }

        case TIME: {
            int senderID = 0;
            time_t rawtime;
            struct tm *timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            msg newMessage = createNewMessage(receivedMessage->receivedFrom, &senderID);
            strcpy(newMessage.messageContent, asctime(timeinfo));

            check_err(msgsnd(senderID, &newMessage, MESSAGE_SIZE, 0));
            break;
        }
        case CALC: {
            int senderID = 0;
            int res;
            msg newMessage = createNewMessage(receivedMessage->receivedFrom, &senderID);

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


            sprintf(newMessage.messageContent, "%d", res);
            check_err(msgsnd(senderID, &newMessage, MESSAGE_SIZE, 0));
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

msg createNewMessage(int receivedFrom, int *currentClientQueueID) {
    msg newMessage;
    *currentClientQueueID = -1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clientPID[i] == receivedFrom) {
            *currentClientQueueID = clientQUEUE[i];
        }
    }
    if(*currentClientQueueID == -1) {
        printf("Don't have such a client..\n");
    };
    newMessage.receivedFrom = getpid();
    return newMessage;
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
